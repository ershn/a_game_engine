#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

#include "GLFW.hpp"
#include "Material.hpp"
#include "Matrix.hpp"
#include "Mesh.hpp"
#include "Transform.hpp"

namespace Age::Gfx
{
struct LocalToViewMatrix
{
    static constexpr auto TYPE{Core::ComponentType::LOCAL_TO_VIEW_MATRIX};

    Math::Matrix4 matrix{};
};

struct LocalToViewNormalMatrix
{
    static constexpr auto TYPE{Core::ComponentType::LOCAL_TO_VIEW_NORMAL_MATRIX};

    Math::Matrix3 matrix{};
};

struct DrawCall
{
    const Math::Matrix4 *lv_matrix{};
    const Math::Matrix3 *lv_normal_matrix{};
    MaterialId material_id{};
    MeshId mesh_id{};
};

using DrawCallIndex = std::uint32_t;
using DrawCallSortKey = std::uint32_t;

struct DrawCallKey
{
    DrawCallIndex index{std::numeric_limits<DrawCallIndex>::max()};
    DrawCallSortKey sort_key{std::numeric_limits<DrawCallSortKey>::max()};
};

struct Renderer
{
    static constexpr auto TYPE{Core::ComponentType::RENDERER};

    DrawCallKey draw_call_key{};
    bool is_active{true};
};

void init_rendering_system(GLFWwindow *window);

inline constexpr unsigned int WITH_LV_MATRIX{0b1};
inline constexpr unsigned int WITH_LV_NORMAL_MATRIX{0b10};

void init_renderer(Core::EntityId entity_id, unsigned int options = 0);

void render();
void update_render_state();
} // namespace Age::Gfx
