#include <algorithm>
#include <cmath>
#include <functional>
#include <tuple>
#include <vector>

#include "Camera.hpp"
#include "Color.hpp"
#include "ECS.hpp"
#include "OpenGL.hpp"
#include "Rendering.hpp"
#include "Transformations.hpp"
#include "Viewport.hpp"

namespace Age::Gfx
{
namespace
{
GLFWwindow *s_window{};

void calc_local_to_view_matrix(
    const Math::Matrix4 &world_to_view_matrix, const Core::Transform &transform, LocalToViewMatrix &local_to_view_matrix
)
{
    Math::Matrix4 local_to_world_matrix{
        Math::translation_matrix(transform.position) * Math::affine_rotation_matrix(transform.orientation) *
        Math::affine_scaling_matrix(transform.scale)
    };
    local_to_view_matrix.matrix = world_to_view_matrix * local_to_world_matrix;
}

void calc_local_to_view_normal_matrix(const LocalToViewMatrix &view_matrix, LocalToViewNormalMatrix &view_normal_matrix)
{
    view_normal_matrix.matrix = Math::Matrix3{view_matrix.matrix}.invert().transpose();
}

void set_global_light_settings(LightsBlock &lights_block, const GlobalLightSettings &global_light_settings)
{
    lights_block.ambient_light_intensity = global_light_settings.ambient_light_intensity;
    lights_block.light_attenuation = global_light_settings.light_attenuation;
    lights_block.max_intensity = global_light_settings.max_intensity;
}

void set_directional_light(
    LightsBlock &lights_block,
    std::size_t &light_index,
    const Math::Matrix4 &world_to_view_matrix,
    const Core::Transform &transform,
    const DirectionalLight &directional_light
)
{
    if (light_index < LightsBlock::LIGHT_COUNT)
    {
        lights_block.lights[light_index++] = {
            .view_position{world_to_view_matrix * Math::Vector4{transform.position, 0.0f}},
            .intensity{directional_light.light_intensity}
        };
    }
}

void set_point_light(
    LightsBlock &lights_block,
    std::size_t &light_index,
    const Math::Matrix4 &world_to_view_matrix,
    const Core::Transform &transform,
    const PointLight &point_light
)
{
    if (light_index < LightsBlock::LIGHT_COUNT)
    {
        lights_block.lights[light_index++] = {
            .view_position{world_to_view_matrix * Math::Vector4{transform.position, 1.0f}},
            .intensity{point_light.light_intensity}
        };
    }
}

constexpr unsigned int MATERIAL_ID_SHIFT_COUNT{16U};
constexpr unsigned int MESH_ID_SHIFT_COUNT{0U};

std::vector<DrawCall> s_draw_calls{};
std::vector<DrawCallKey> s_draw_call_keys{};

GLuint s_bound_vao{};

void init_renderer(
    Renderer &renderer,
    const Math::Matrix4 *local_to_view_matrix,
    const Math::Matrix3 *local_to_view_normal_matrix,
    const UniformBufferRangeBind *buffer_range_bind,
    MaterialId material_id,
    MeshId mesh_id
)
{
    s_draw_calls.emplace_back(
        local_to_view_matrix, local_to_view_normal_matrix, buffer_range_bind, material_id, mesh_id
    );

    renderer.draw_call_key.index = static_cast<DrawCallIndex>(s_draw_calls.size() - 1);
    renderer.draw_call_key.sort_key = material_id << MATERIAL_ID_SHIFT_COUNT & mesh_id << MESH_ID_SHIFT_COUNT;
    renderer.is_active = true;
}

void enqueue_draw_calls(const Renderer &renderer)
{
    if (renderer.is_active)
        s_draw_call_keys.emplace_back(renderer.draw_call_key);
}

void sort_draw_calls()
{
    s_draw_call_keys.clear();
    Core::process_components(enqueue_draw_calls);
    std::sort(s_draw_call_keys.begin(), s_draw_call_keys.end(), [](const DrawCallKey &lhs, const DrawCallKey &rhs) {
        return lhs.sort_key < rhs.sort_key;
    });
}

void render_entities_to_camera(
    const CameraRenderState &camera_render_state,
    const WorldToViewMatrix &view_matrix,
    const ProjectionBufferBlockRef &projection_buffer_block,
    const LightsBufferBlockRef &lights_buffer_block
)
{
    {
        LightsBlock lights_block;
        std::size_t light_index{};

        Core::process_components(
            std::function<void(const GlobalLightSettings &)>{
                std::bind_front(set_global_light_settings, std::ref(lights_block))
            }
        );
        Core::process_components(
            std::function<void(const Core::Transform &, const DirectionalLight &)>{std::bind_front(
                set_directional_light, std::ref(lights_block), std::ref(light_index), std::cref(view_matrix.matrix)
            )}
        );
        Core::process_components(
            std::function<void(const Core::Transform &, const PointLight &)>{std::bind_front(
                set_point_light, std::ref(lights_block), std::ref(light_index), std::cref(view_matrix.matrix)
            )}
        );

        lights_buffer_block = lights_block;
    }

    {
        const Viewport &viewport{use_viewport(camera_render_state.viewport_id)};

        bool is_custom_viewport{camera_render_state.viewport_id != FULL_VIEWPORT_ID};
        if (is_custom_viewport)
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(viewport.origin_x, viewport.origin_y, viewport.width, viewport.height);
        }

        GLbitfield cleared_buffers{};
        if (camera_render_state.flags & CLEAR_COLOR_BUFFER)
        {
            cleared_buffers |= GL_COLOR_BUFFER_BIT;
            OGL::set_clear_color(camera_render_state.clear_color);
        }
        if (camera_render_state.flags & CLEAR_DEPTH_BUFFER)
        {
            cleared_buffers |= GL_DEPTH_BUFFER_BIT;
            OGL::set_clear_depth(camera_render_state.clear_depth);
        }
        if (cleared_buffers)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (is_custom_viewport)
            glDisable(GL_SCISSOR_TEST);
    }

    if (camera_render_state.flags & DEPTH_CLAMPING)
        glEnable(GL_DEPTH_CLAMP);

    bind_uniform_buffer(PROJECTION_BLOCK_BINDING, projection_buffer_block.get_buffer_range());
    bind_uniform_buffer(LIGHT_DATA_BLOCK_BINDING, lights_buffer_block.get_buffer_range());

    Core::process_components(
        std::function<void(const Core::Transform &, LocalToViewMatrix &)>{
            std::bind_front(calc_local_to_view_matrix, std::cref(view_matrix.matrix))
        }
    );
    Core::process_components(calc_local_to_view_normal_matrix);

    for (DrawCallKey draw_call_key : s_draw_call_keys)
    {
        const DrawCall &draw_call{s_draw_calls[draw_call_key.index]};

        if (draw_call.uniform_buffer_range_bind != nullptr)
            bind_uniform_buffer(*draw_call.uniform_buffer_range_bind);

        const Material &material{use_material(draw_call.material_id)};
        if (draw_call.local_to_view_matrix != nullptr)
            OGL::set_uniform(material.shader.local_to_view_matrix, *draw_call.local_to_view_matrix);
        if (draw_call.local_to_view_normal_matrix != nullptr)
            OGL::set_uniform(material.shader.local_to_view_normal_matrix, *draw_call.local_to_view_normal_matrix);

        MeshDrawCommands mesh_draw_commands{get_mesh_draw_commands(draw_call.mesh_id)};
        if (mesh_draw_commands.vertex_array_object != s_bound_vao)
        {
            OGL::bind_vertex_array_object(mesh_draw_commands.vertex_array_object);
            s_bound_vao = mesh_draw_commands.vertex_array_object;
        }
        for (const DrawCommand &draw_command : mesh_draw_commands.draw_commands)
        {
            switch (draw_command.type)
            {
            case DrawCommandType::DRAW_ARRAYS:
                OGL::draw_arrays(draw_command.rendering_mode, draw_command.element_count, draw_command.offset);
                break;
            case DrawCommandType::DRAW_ELEMENTS:
                OGL::draw_elements(draw_command.rendering_mode, draw_command.element_count, draw_command.offset);
                break;
            }
        }
    }

    if (camera_render_state.flags & DEPTH_CLAMPING)
        glDisable(GL_DEPTH_CLAMP);
}
} // namespace

void init_rendering_system(GLFWwindow *window)
{
    s_window = window;

    s_draw_calls.reserve(2048);
    s_draw_call_keys.reserve(2048);

    init_viewport_system(window);
    init_mesh_system();
    init_shader_system();
    init_material_system();
    init_uniform_buffer_system();

    glfwSwapInterval(1);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0, 1.0);
}

void init_renderer(Core::EntityId entity_id, unsigned int options)
{
    auto [renderer, material, mesh] =
        Core::get_entity_components<Renderer, const MaterialRef, const MeshRef>(entity_id);

    const Math::Matrix4 *lv_matrix{};
    if (options & RENDER_WITH_LV_MATRIX)
        lv_matrix = &Core::get_entity_component<const LocalToViewMatrix>(entity_id).matrix;

    const Math::Matrix3 *lv_normal_matrix{};
    if (options & RENDER_WITH_LV_NORMAL_MATRIX)
        lv_normal_matrix = &Core::get_entity_component<const LocalToViewNormalMatrix>(entity_id).matrix;

    const UniformBufferRangeBind *uniform_buffer_range_bind{};
    if (options & RENDER_WITH_BUFFER_RANGE_BIND)
        uniform_buffer_range_bind = &Core::get_entity_component<const UniformBufferRangeBind>(entity_id);

    init_renderer(renderer, lv_matrix, lv_normal_matrix, uniform_buffer_range_bind, material.material_id, mesh.mesh_id);
}

void render()
{
    end_viewports_update();

    sort_draw_calls();
    Core::process_components(render_entities_to_camera);

    glfwSwapBuffers(s_window);
}

void update_render_state()
{
    start_viewports_update();
}
} // namespace Age::Gfx
