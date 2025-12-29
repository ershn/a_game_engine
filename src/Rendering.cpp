#include <algorithm>
#include <cmath>
#include <functional>
#include <tuple>
#include <vector>

#include "Camera.hpp"
#include "Color.hpp"
#include "ECS.hpp"
#include "Lighting.hpp"
#include "OpenGL.hpp"
#include "Rendering.hpp"
#include "Texture.hpp"
#include "Transformations.hpp"
#include "Viewport.hpp"

namespace Age::Gfx
{
namespace
{
GLFWwindow *s_window{};

void calc_local_to_view_matrix(
    const Math::Matrix4 &wv_matrix, const Core::Transform &transform, LocalToViewMatrix &lv_matrix
)
{
    lv_matrix.matrix = wv_matrix * Core::transform_matrix(transform);
}

void calc_local_to_view_normal_matrix(const LocalToViewMatrix &lv_matrix, LocalToViewNormalMatrix &lv_normal_matrix)
{
    lv_normal_matrix.matrix = Math::Matrix3{lv_matrix.matrix}.invert().transpose();
}

constexpr unsigned int MATERIAL_ID_SHIFT_COUNT{16U};
constexpr unsigned int MESH_ID_SHIFT_COUNT{0U};

std::vector<DrawCall> s_draw_calls{};
std::vector<DrawCallKey> s_draw_call_keys{};

GLuint s_bound_vao{};

void init_renderer(
    Renderer &renderer,
    const Math::Matrix4 *lv_matrix,
    const Math::Matrix3 *lv_normal_matrix,
    MaterialId material_id,
    MeshId mesh_id
)
{
    s_draw_calls.emplace_back(lv_matrix, lv_normal_matrix, material_id, mesh_id);

    renderer.draw_call_key = {
        .index = static_cast<DrawCallIndex>(s_draw_calls.size() - 1),
        .sort_key = material_id << MATERIAL_ID_SHIFT_COUNT | mesh_id << MESH_ID_SHIFT_COUNT
    };
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

void render_camera(
    const CameraRenderState &camera_render_state,
    const WorldToViewMatrix &wv_matrix,
    const ProjectionUniformBuffer &projection_buffer
)
{
    Core::process_components(std::function{[&](const Core::Transform &transform, LocalToViewMatrix &lv_matrix) {
        calc_local_to_view_matrix(wv_matrix.matrix, transform, lv_matrix);
    }});
    Core::process_components(calc_local_to_view_normal_matrix);

    Core::process_components(std::function{[&](const LightGroup &light_group) {
        update_light_group_buffer(wv_matrix.matrix, light_group);
    }});

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
            glClear(cleared_buffers);

        if (is_custom_viewport)
            glDisable(GL_SCISSOR_TEST);
    }

    if (camera_render_state.flags & DEPTH_CLAMPING)
        glEnable(GL_DEPTH_CLAMP);

    for (DrawCallKey draw_call_key : s_draw_call_keys)
    {
        const DrawCall &draw_call{s_draw_calls[draw_call_key.index]};
        const Material &material{use_material(draw_call.material_id)};
        Shader &shader{material.shader};

        if (is_uniform_block_defined(shader.projection_block))
        {
            bind_uniform_buffer_range(
                shader.shader_program, shader.projection_block, projection_buffer.buffer_range_id
            );
        }

        if (draw_call.lv_matrix != nullptr)
            OGL::set_uniform(shader.lv_matrix, *draw_call.lv_matrix);
        if (draw_call.lv_normal_matrix != nullptr)
            OGL::set_uniform(shader.lv_normal_matrix, *draw_call.lv_normal_matrix);

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
    init_texture_system();

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
    if (options & WITH_LV_MATRIX)
        lv_matrix = &Core::get_entity_component<const LocalToViewMatrix>(entity_id).matrix;

    const Math::Matrix3 *lv_normal_matrix{};
    if (options & WITH_LV_NORMAL_MATRIX)
        lv_normal_matrix = &Core::get_entity_component<const LocalToViewNormalMatrix>(entity_id).matrix;

    init_renderer(renderer, lv_matrix, lv_normal_matrix, material.material_id, mesh.mesh_id);
}

void render()
{
    end_viewports_update();

    sort_draw_calls();
    Core::process_components(render_camera);
    release_used_material();

    glfwSwapBuffers(s_window);
}

void update_render_state()
{
    start_viewports_update();
}
} // namespace Age::Gfx
