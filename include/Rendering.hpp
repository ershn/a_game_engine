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

struct PointLight
{
    static constexpr auto TYPE{Core::ComponentType::POINT_LIGHT};

    Math::Vector4 light_intensity{};
    float light_attenuation{};
    Math::Vector4 ambient_light_intensity{};
};

struct LightDataBufferBlock : public UniformBufferBlock<LightDataUniformBlock>
{
    static constexpr auto TYPE{Core::ComponentType::LIGHT_DATA_BUFFER_BLOCK};

    using UniformBufferBlock<LightDataUniformBlock>::operator=;
};

struct DrawCall
{
    const Math::Matrix4 &local_to_view_matrix;
    const Math::Matrix3 *local_to_view_normal_matrix{};
    const UniformBufferRangeBind *uniform_buffer_range_bind{};
    MaterialId material_id{};
    ModelId model_id{};
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

void init_renderer(Renderer &renderer,
                   const Math::Matrix4 &local_to_view_matrix,
                   const Math::Matrix3 *local_to_view_normal_matrix,
                   const UniformBufferRangeBind *buffer_block_bind,
                   MaterialId material_id,
                   ModelId model_id);

inline constexpr unsigned int RENDER_WITH_NORMAL_MATRIX{0b1};
inline constexpr unsigned int RENDER_WITH_BUFFER_RANGE_BIND{0b10};

void init_renderer(Core::EntityId entity_id, unsigned int options = 0);

void render();
} // namespace Age::Gfx
