#include <algorithm>
#include <vector>

#include "ecs.hpp"
#include "framebuffer.hpp"
#include "lighting.hpp"
#include "multi_span.hpp"
#include "opengl/opengl_api.hpp"
#include "rendering.hpp"
#include "texture.hpp"
#include "viewport.hpp"

namespace Age::Gfx
{
namespace
{
GLFWwindow *s_window{};

std::vector<DrawCall> s_draw_calls{};

std::vector<std::uint32_t> s_dc_index_in_layer{};
std::array<std::vector<DrawCallKey>, 64> s_dc_keys_by_layer{};

GLuint s_bound_vao{};

constexpr std::size_t to_index(Layer layer)
{
    return static_cast<std::size_t>(layer);
}

void add_draw_call_to_layer(DrawCallKey draw_call_key, Layer layer)
{
    auto &layer_dc_keys = s_dc_keys_by_layer[to_index(layer)];
    layer_dc_keys.emplace_back(draw_call_key);
    s_dc_index_in_layer[draw_call_key.index] = static_cast<std::uint32_t>(layer_dc_keys.size() - 1);
}

void remove_draw_call_from_layer(DrawCallKey draw_call_key, Layer layer)
{
    auto &layer_dc_keys = s_dc_keys_by_layer[to_index(layer)];
    if (layer_dc_keys.size() > 1)
        layer_dc_keys[s_dc_index_in_layer[draw_call_key.index]] = layer_dc_keys.back();
    layer_dc_keys.pop_back();
    s_dc_index_in_layer[draw_call_key.index] = std::numeric_limits<std::uint32_t>::max();
}

DrawCallSortKey create_sort_key(MaterialId material_id, MeshId mesh_id)
{
    DrawCallSortKey sort_key{};
    unsigned int bit_offset{0};

    sort_key |= static_cast<DrawCallSortKey>(mesh_id) << bit_offset;
    bit_offset += sizeof(MeshId) * 8;
    sort_key |= static_cast<DrawCallSortKey>(material_id) << bit_offset;
    bit_offset += sizeof(MaterialId) * 8;
    DrawQueue draw_queue{get_material(material_id).draw_queue};
    sort_key |= static_cast<DrawCallSortKey>(draw_queue) << bit_offset;
    bit_offset += DrawQueue::BitWidth::TOTAL;

    return sort_key;
}

DrawQueue get_draw_queue(DrawCallSortKey sort_key)
{
    unsigned int draw_queue{static_cast<unsigned int>(sort_key >> (sizeof(MeshId) * 8 + sizeof(MaterialId) * 8))};
    return DrawQueue::from(draw_queue);
}

void init_renderer(Renderer &renderer, const Math::Matrix4 *lw_matrix, MaterialId material_id, MeshId mesh_id)
{
    s_draw_calls.emplace_back(lw_matrix, material_id, mesh_id);
    s_dc_index_in_layer.emplace_back(std::numeric_limits<std::uint32_t>::max());

    renderer.draw_call_key = {
        .sort_key = create_sort_key(material_id, mesh_id),
        .index = static_cast<DrawCallIndex>(s_draw_calls.size() - 1),
    };

    add_draw_call_to_layer(renderer.draw_call_key, renderer.layer);
}

void calc_local_to_world_matrix(const Core::Transform &transform, LocalToWorldMatrix &lw_matrix)
{
    lw_matrix.matrix = Core::transform_matrix(transform);
}

void execute_draw_call(
    DrawCallIndex draw_call_index, const WorldToViewMatrix &wv_matrix, const ProjectionUniformBuffer &projection_buffer
)
{
    const DrawCall &draw_call{s_draw_calls[draw_call_index]};
    const Material &material{use_material(draw_call.material_id)};
    Shader &shader{material.shader};

    if (is_uniform_block_defined(shader.projection_block))
        bind_uniform_buffer_range(shader.shader_program, shader.projection_block, projection_buffer.buffer_range_id);

    if (draw_call.lw_matrix != nullptr)
    {
        Math::Matrix4 lv_matrix{wv_matrix.matrix * *draw_call.lw_matrix};
        OGL::set_uniform(shader.lv_matrix, lv_matrix);

        if (shader.lv_normal_matrix != -1)
        {
            Math::Matrix3 lv_normal_matrix{Math::Matrix3{lv_matrix}.invert().transpose()};
            OGL::set_uniform(shader.lv_normal_matrix, lv_normal_matrix);
        }
    }

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
} // namespace

void init_rendering_system(GLFWwindow *window)
{
    s_window = window;

    s_draw_calls.reserve(2048);

    init_framebuffer_system(window);
    init_viewport_system();
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

    const Math::Matrix4 *lw_matrix{};
    if (options & WITH_LW_MATRIX)
        lw_matrix = &Core::get_entity_component<const LocalToWorldMatrix>(entity_id).matrix;

    init_renderer(renderer, lw_matrix, material.material_id, mesh.mesh_id);
}

void enable_renderer(Renderer &renderer)
{
    if (renderer.enabled)
        return;

    add_draw_call_to_layer(renderer.draw_call_key, renderer.layer);
    renderer.enabled = true;
}

void disable_renderer(Renderer &renderer)
{
    if (renderer.enabled == false)
        return;

    remove_draw_call_from_layer(renderer.draw_call_key, renderer.layer);
    renderer.enabled = false;
}

void set_renderer_layer(Renderer &renderer, Layer layer)
{
    if (renderer.layer == layer)
        return;

    if (renderer.enabled)
    {
        remove_draw_call_from_layer(renderer.draw_call_key, renderer.layer);
        add_draw_call_to_layer(renderer.draw_call_key, layer);
    }

    renderer.layer = layer;
}

void update_lighting(const WorldToViewMatrix &wv_matrix)
{
    Core::process_components([&](const LightGroup &light_group) {
        update_light_group_buffer(wv_matrix.matrix, light_group);
    });
}

void setup_viewport(const CameraRenderState &camera_render_state, const CameraClear &camera_clear)
{
    const Math::Vector2U &framebuffer_size{get_framebuffer_size(camera_render_state.framebuffer_id)};

    ViewportId viewport_id{camera_render_state.viewport_id};
    Math::RectangleI viewport_pixel_rect{calc_viewport_pixel_rect(get_viewport(viewport_id), framebuffer_size)};
    use_viewport_pixel_rect(viewport_pixel_rect);

    if (camera_clear.framebuffer_clear.buffers)
    {
        bool is_custom_viewport{viewport_id != FULL_VIEWPORT_ID};
        if (is_custom_viewport)
        {
            OGL::enable_scissor_test(true);
            OGL::set_scissor(viewport_pixel_rect);
        }

        clear_framebuffer(camera_render_state.framebuffer_id, camera_clear.framebuffer_clear);

        if (is_custom_viewport)
            OGL::enable_scissor_test(false);
    }
}

std::vector<DrawCallKey> &get_layer_draw_calls(Layer layer)
{
    return s_dc_keys_by_layer[to_index(layer)];
}

void sort_draw_calls(std::vector<DrawCallKey> &draw_call_keys)
{
    std::sort(draw_call_keys.begin(), draw_call_keys.end(), [](const DrawCallKey &lhs, const DrawCallKey &rhs) {
        return lhs.sort_key < rhs.sort_key;
    });
}

template <>
std::vector<DrawCallKey>::const_iterator execute_draw_calls<Util::Less>(
    DrawQueue max_draw_queue,
    std::vector<DrawCallKey>::const_iterator dc_key_it,
    std::vector<DrawCallKey>::const_iterator dc_key_end,
    const WorldToViewMatrix &wv_matrix,
    const ProjectionUniformBuffer &projection_buffer
)
{
    for (; dc_key_it != dc_key_end && get_draw_queue(dc_key_it->sort_key) < max_draw_queue; ++dc_key_it)
        execute_draw_call(dc_key_it->index, wv_matrix, projection_buffer);

    return dc_key_it;
}

template <>
std::vector<DrawCallKey>::const_iterator execute_draw_calls<Util::LessEqual>(
    DrawQueue max_draw_queue,
    std::vector<DrawCallKey>::const_iterator dc_key_it,
    std::vector<DrawCallKey>::const_iterator dc_key_end,
    const WorldToViewMatrix &wv_matrix,
    const ProjectionUniformBuffer &projection_buffer
)
{
    for (; dc_key_it != dc_key_end && get_draw_queue(dc_key_it->sort_key) <= max_draw_queue; ++dc_key_it)
        execute_draw_call(dc_key_it->index, wv_matrix, projection_buffer);

    return dc_key_it;
}

// Rendering flow
// for each camera:
//    1. select the list of renderers matching the camera layer
//    2. perform culling on the renderers
//    3. sort renderers:
//       1. render queue
//       2. state change
//       3. distance from camera (front to back, back to front, no sorting)
//    4. render each render queue successively

void prepare_rendering()
{
    Core::process_components(calc_local_to_world_matrix);
}

void render_scene()
{
    using Cameras = Core::
        MultiSpan<const CameraRenderState, const CameraClear, const WorldToViewMatrix, const ProjectionUniformBuffer>;

    Core::execute([](const Cameras &cameras) {
        for (const auto &[camera_render_state, camera_clear, wv_matrix, projection_buffer] : cameras)
        {
            update_lighting(wv_matrix);
            setup_viewport(camera_render_state, camera_clear);

            auto &draw_call_keys = get_layer_draw_calls(camera_render_state.layer);
            sort_draw_calls(draw_call_keys);

            auto dc_key_it = draw_call_keys.cbegin();
            auto dc_key_end = draw_call_keys.cend();
            execute_draw_calls<Util::LessEqual>(DrawQueue::max, dc_key_it, dc_key_end, wv_matrix, projection_buffer);
        }
    });
}

void complete_rendering()
{
    release_used_material();

    glfwSwapBuffers(s_window);
}

void update_render_state()
{
    update_system_framebuffer_size();
}
} // namespace Age::Gfx
