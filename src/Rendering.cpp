#include <algorithm>
#include <functional>
#include <vector>

#include "Camera.hpp"
#include "OpenGL.hpp"
#include "Rendering.hpp"
#include "Transformations.hpp"
#include "UniformBufferInstances.hpp"

namespace Age::Gfx
{
namespace
{
GLFWwindow *s_window{};

////////////////////////////////////////

bool s_framebuffer_size_changed{};
int s_framebuffer_width{};
int s_framebuffer_height{};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    s_framebuffer_width = width;
    s_framebuffer_height = height;
    s_framebuffer_size_changed = true;
}

void init_viewport()
{
    glfwGetFramebufferSize(s_window, &s_framebuffer_width, &s_framebuffer_height);
    s_framebuffer_size_changed = true;

    glfwSetFramebufferSizeCallback(s_window, framebuffer_size_callback);
}

void update_viewport()
{
    if (s_framebuffer_size_changed == false)
        return;

    s_framebuffer_size_changed = false;
    glViewport(0, 0, s_framebuffer_width, s_framebuffer_height);

    float aspect_ratio{static_cast<float>(s_framebuffer_width) / s_framebuffer_height};
    update_cameras_aspect_ratio(aspect_ratio);
}

////////////////////////////////////////

void calc_model_to_camera_matrix(const Math::Matrix4 &world_to_camera_matrix,
                                 const Core::Transform &transform,
                                 ModelToCameraMatrix &model_to_camera_matrix)
{
    Math::Matrix4 model_to_world_matrix{Math::translation_matrix(transform.position) *
                                        Math::affine_rotation_matrix(transform.orientation) *
                                        Math::affine_scaling_matrix(transform.scale)};
    model_to_camera_matrix.matrix = world_to_camera_matrix * model_to_world_matrix;
}

void calc_model_to_camera_normal_matrix(const ModelToCameraMatrix &camera_matrix,
                                        ModelToCameraNormalMatrix &camera_normal_matrix)
{
    camera_normal_matrix.matrix = Math::Matrix3{camera_matrix.matrix}.invert().transpose();
}

void write_point_light_to_buffer(const Math::Matrix4 &world_to_camera_matrix,
                                 const LightDataUniformBuffer &light_data_buffer,
                                 const Core::Transform &transform,
                                 const PointLight &point_light)
{
    light_data_buffer.set_camera_light_position(
        Math::Vector3{world_to_camera_matrix * Math::Vector4{transform.position, 1.0f}});
    light_data_buffer.set_light_intensity(point_light.light_intensity);
    light_data_buffer.set_light_attenuation(point_light.light_attenuation);
    light_data_buffer.set_ambient_light_intensity(point_light.ambient_light_intensity);
}

////////////////////////////////////////

constexpr unsigned int MATERIAL_ID_SHIFT_COUNT{16U};
constexpr unsigned int MODEL_ID_SHIFT_COUNT{0U};

std::vector<DrawCall> s_draw_calls{};
std::vector<DrawCallKey> s_draw_call_keys{};

GLuint s_bound_vao{};

void init_renderer(Renderer &renderer,
                   const Math::Matrix4 &model_to_camera_matrix,
                   const Math::Matrix3 *model_to_camera_normal_matrix,
                   MaterialId material_id,
                   ModelId model_id)
{
    s_draw_calls.emplace_back(model_to_camera_matrix, model_to_camera_normal_matrix, material_id, model_id);

    renderer.draw_call_key.index = static_cast<DrawCallIndex>(s_draw_calls.size() - 1);
    renderer.draw_call_key.sort_key = material_id << MATERIAL_ID_SHIFT_COUNT & model_id << MODEL_ID_SHIFT_COUNT;
    renderer.is_active = true;
}

void enqueue_draw_calls(const Renderer &renderer)
{
    if (renderer.is_active)
        s_draw_call_keys.emplace_back(renderer.draw_call_key);
}

void render_entities_to_camera(const WorldToCameraMatrix &camera_matrix,
                               const ProjectionBufferRef &projection_buffer_ref,
                               const LightDataBufferRef &light_data_buffer_ref)
{
    const auto &light_data_buffer =
        static_cast<const LightDataUniformBuffer &>(get_uniform_buffer(light_data_buffer_ref.buffer_id));

    bind_uniform_buffer(get_uniform_buffer(projection_buffer_ref.buffer_id), PROJECTION_BLOCK_BINDING);
    bind_uniform_buffer(light_data_buffer, LIGHT_DATA_BLOCK_BINDING);

    Core::process_components(std::function<void(const Core::Transform &, const PointLight &)>{
        std::bind_front(write_point_light_to_buffer, camera_matrix.matrix, light_data_buffer)});

    Core::process_components(std::function<void(const Core::Transform &, ModelToCameraMatrix &)>{
        std::bind_front(calc_model_to_camera_matrix, camera_matrix.matrix)});
    Core::process_components(calc_model_to_camera_normal_matrix);

    for (DrawCallKey draw_call_key : s_draw_call_keys)
    {
        const DrawCall &draw_call{s_draw_calls[draw_call_key.index]};

        const Material &material{use_material(draw_call.material_id)};
        set_uniform(material.shader.model_to_camera_matrix, draw_call.model_to_camera_matrix);
        if (draw_call.model_to_camera_normal_matrix != nullptr)
            set_uniform(material.shader.model_to_camera_normal_matrix, *draw_call.model_to_camera_normal_matrix);

        DrawCommand draw_command{get_draw_command(draw_call.model_id)};
        if (draw_command.vertex_array_object != s_bound_vao)
        {
            bind_vertex_array_object(draw_command.vertex_array_object);
            s_bound_vao = draw_command.vertex_array_object;
        }
        for (const Mesh &mesh : draw_command.meshes)
        {
            draw_elements(mesh.rendering_mode, mesh.element_count, mesh.buffer_offset);
        }
    }
}

void render_entities()
{
    s_draw_call_keys.clear();
    Core::process_components(enqueue_draw_calls);
    std::sort(s_draw_call_keys.begin(), s_draw_call_keys.end(),
              [](const DrawCallKey &lhs, const DrawCallKey &rhs) { return lhs.sort_key < rhs.sort_key; });

    Core::process_components(render_entities_to_camera);
}
} // namespace

void init_rendering_system(GLFWwindow *window)
{
    s_window = window;

    s_draw_calls.reserve(2048);
    s_draw_call_keys.reserve(2048);

    init_mesh_system();
    init_shader_system();
    init_material_system();
    init_uniform_buffer_system();

    init_viewport();

    glfwSwapInterval(1);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0, 1.0);
}

void init_renderer(Renderer &renderer,
                   const Math::Matrix4 &model_to_camera_matrix,
                   MaterialId material_id,
                   ModelId model_id)
{
    init_renderer(renderer, model_to_camera_matrix, nullptr, material_id, model_id);
}

void init_renderer(Renderer &renderer,
                   const Math::Matrix4 &model_to_camera_matrix,
                   const Math::Matrix3 &model_to_camera_normal_matrix,
                   MaterialId material_id,
                   ModelId model_id)
{
    init_renderer(renderer, model_to_camera_matrix, &model_to_camera_normal_matrix, material_id, model_id);
}

void render()
{
    update_viewport();

    glClearColor(0.294f, 0.22f, 0.192f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_entities();

    glfwSwapBuffers(s_window);
}
} // namespace Age::Gfx
