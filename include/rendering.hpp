#pragma once

#include <cstdint>
#include <limits>

#include "camera.hpp"
#include "comparisons.hpp"
#include "glfw.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "mesh.hpp"

namespace Age::Gfx
{
struct LocalToWorldMatrix
{
    static constexpr auto TYPE{Core::ComponentType::LOCAL_TO_WORLD_MATRIX};

    Math::Matrix4 matrix{};
};

struct DrawCall
{
    const Math::Matrix4 *lw_matrix{};
    MaterialId material_id{};
    MeshId mesh_id{};
};

using DrawCallSortKey = std::uint64_t;
using DrawCallIndex = std::uint32_t;

struct DrawCallKey
{
    DrawCallSortKey sort_key{std::numeric_limits<DrawCallSortKey>::max()};
    DrawCallIndex index{std::numeric_limits<DrawCallIndex>::max()};
};

struct Renderer
{
    static constexpr auto TYPE{Core::ComponentType::RENDERER};

    DrawCallKey draw_call_key{};
    Layer layer{};
    bool enabled{true};
};

using RenderScene = void (*)();

void init_rendering_system(GLFWwindow *window);

inline constexpr unsigned int WITH_LW_MATRIX{0b1};

void init_renderer(Core::EntityId entity_id, unsigned int options = 0);

void enable_renderer(Renderer &renderer);
void disable_renderer(Renderer &renderer);
void set_renderer_layer(Renderer &renderer, Layer layer);

void update_lighting(const WorldToViewMatrix &wv_matrix);
void setup_viewport(const CameraRenderState &camera_render_state);

std::vector<DrawCallKey> &get_layer_draw_calls(Layer layer);
void sort_draw_calls(std::vector<DrawCallKey> &draw_call_keys);

template <typename TDrawQueueCmp>
std::vector<DrawCallKey>::const_iterator execute_draw_calls(
    DrawQueue max_draw_queue,
    std::vector<DrawCallKey>::const_iterator dc_key_it,
    std::vector<DrawCallKey>::const_iterator dc_key_end,
    const WorldToViewMatrix &wv_matrix,
    const ProjectionUniformBuffer &projection_buffer
) = delete;

template <>
std::vector<DrawCallKey>::const_iterator execute_draw_calls<Util::Less>(
    DrawQueue max_draw_queue,
    std::vector<DrawCallKey>::const_iterator dc_key_it,
    std::vector<DrawCallKey>::const_iterator dc_key_end,
    const WorldToViewMatrix &wv_matrix,
    const ProjectionUniformBuffer &projection_buffer
);

template <>
std::vector<DrawCallKey>::const_iterator execute_draw_calls<Util::LessEqual>(
    DrawQueue max_draw_queue,
    std::vector<DrawCallKey>::const_iterator dc_key_it,
    std::vector<DrawCallKey>::const_iterator dc_key_end,
    const WorldToViewMatrix &wv_matrix,
    const ProjectionUniformBuffer &projection_buffer
);

void prepare_rendering();
void render_scene();
void complete_rendering();

template <RenderScene RenderScene>
void render()
{
    prepare_rendering();
    RenderScene();
    complete_rendering();
}

void update_render_state();
} // namespace Age::Gfx
