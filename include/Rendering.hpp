#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

#include "ECS.hpp"
#include "GLFW.hpp"
#include "Material.hpp"
#include "Matrix.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Transform.hpp"
#include "UniformBlocks.hpp"
#include "UniformBuffer.hpp"

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

struct GlobalLightSettings
{
    static constexpr auto TYPE{Core::ComponentType::GLOBAL_LIGHT_SETTINGS};

    Math::Vector4 ambient_light_intensity{};
    float light_attenuation{};
    float max_intensity{};
};

struct DirectionalLight
{
    static constexpr auto TYPE{Core::ComponentType::DIRECTIONAL_LIGHT};

    Math::Vector4 light_intensity{};
};

struct PointLight
{
    static constexpr auto TYPE{Core::ComponentType::POINT_LIGHT};

    Math::Vector4 light_intensity{};
};

struct LightsBufferBlock : public UniformBufferBlock<LightsUniformBlock>
{
    static constexpr auto TYPE{Core::ComponentType::LIGHTS_BUFFER_BLOCK};

    using UniformBufferBlock<LightsUniformBlock>::operator=;
};

struct RenderState
{
    static constexpr auto TYPE{Core::ComponentType::RENDER_STATE};

    Math::Vector4 clear_color;
};

struct DrawCall
{
    const Math::Matrix4 &local_to_view_matrix;
    const Math::Matrix3 *local_to_view_normal_matrix{};
    const UniformBufferRangeBind *uniform_buffer_range_bind{};
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
    bool is_active{};
};

void init_rendering_system(GLFWwindow *window);

inline constexpr unsigned int RENDER_WITH_NORMAL_MATRIX{0b1};
inline constexpr unsigned int RENDER_WITH_BUFFER_RANGE_BIND{0b10};

void init_renderer(Core::EntityId entity_id, unsigned int options = 0);

void render();
} // namespace Age::Gfx
