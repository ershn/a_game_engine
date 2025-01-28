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
#include "UniformBuffer.hpp"

namespace Age::Gfx
{
struct ModelToCameraMatrix
{
    static constexpr auto TYPE{Core::ComponentType::MODEL_TO_CAMERA_MATRIX};

    Math::Matrix4 matrix{};
};

struct ModelToCameraNormalMatrix
{
    static constexpr auto TYPE{Core::ComponentType::MODEL_TO_CAMERA_NORMAL_MATRIX};

    Math::Matrix3 matrix{};
};

struct PointLight
{
    static constexpr auto TYPE{Core::ComponentType::POINT_LIGHT};

    Math::Vector4 light_intensity{};
    float light_attenuation{};
    Math::Vector4 ambient_light_intensity{};
};

struct LightDataBufferRef
{
    static constexpr auto TYPE{Core::ComponentType::LIGHT_DATA_BUFFER_REF};

    UniformBufferId buffer_id{};
};

struct DrawCall
{
    const Math::Matrix4 &model_to_camera_matrix;
    const Math::Matrix3 *model_to_camera_normal_matrix;
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
                   const ModelToCameraMatrix &model_to_camera_matrix,
                   const MaterialRef &material,
                   const ModelRef &model);
void init_renderer(Renderer &renderer,
                   const ModelToCameraMatrix &model_to_camera_matrix,
                   const ModelToCameraNormalMatrix &model_to_camera_normal_matrix,
                   const MaterialRef &material,
                   const ModelRef &model);

template <bool WithNormalMatrix>
void init_renderer(Core::EntityId entity_id)
{
    if constexpr (WithNormalMatrix)
    {
        auto [renderer, model_to_camera_matrix, model_to_camera_normal_matrix, material, model] =
            Core::get_entity_components<Renderer, const ModelToCameraMatrix, const ModelToCameraNormalMatrix,
                                        const MaterialRef, const ModelRef>(entity_id);
        init_renderer(renderer, model_to_camera_matrix, model_to_camera_normal_matrix, material, model);
    }
    else
    {
        auto [renderer, model_to_camera_matrix, material, model] =
            Core::get_entity_components<Renderer, const ModelToCameraMatrix, const MaterialRef, const ModelRef>(
                entity_id);
        init_renderer(renderer, model_to_camera_matrix, material, model);
    }
}

void render();
} // namespace Age::Gfx
