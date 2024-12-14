#pragma once

#include <cstddef>
#include <cstdint>

#include "glad/gl.h"

#include "Components.hpp"
#include "ECS.hpp"
#include "Matrix.hpp"
#include "Mesh.hpp"
#include "Quaternion.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
struct ModelToCameraMatrixComponent
{
    static constexpr ECS::ComponentType TYPE{ECS::ComponentType::MODEL_TO_CAMERA_MATRIX};

    Math::Matrix4 modelToCameraMatrix{};
};

void calc_model_to_camera_matrices(const ECS::TransformComponent &transform, ModelToCameraMatrixComponent &matrix);

struct MaterialPropertiesComponent
{
    static constexpr ECS::ComponentType TYPE{ECS::ComponentType::MATERIAL_PROPERTIES};

    Math::Vector3 diffuse_color{};
    float shininess_factor{};
};

struct ShaderComponent
{
    static constexpr ECS::ComponentType TYPE{ECS::ComponentType::SHADER};

    GLuint shader_program{};
};

struct ModelComponent
{
    static constexpr ECS::ComponentType TYPE{ECS::ComponentType::MODEL};

    ModelId model_id{};
};

struct DrawCall
{
    const ECS::EntityId entity_id{};
    const Math::Vector3 diffuse_color{};
    const float shininess_factor{};
    const GLuint shader_program{};
    const GLuint vertex_array_object{};
    const IModel *draw_command{};
    const ModelType model_type{};
};

constexpr std::uint64_t SHADER_PROGRAM_ID_MASK{0xffff'ffff'0000'0000};
constexpr std::uint64_t VERTEX_ARRAY_OBJECT_ID_MASK{0xffff'ffff};

struct DrawCallKey
{
    const DrawCall &draw_call;
    const std::uint64_t sort_key{};
};

struct RenderComponent
{
    static constexpr ECS::ComponentType TYPE{ECS::ComponentType::RENDER};

    const DrawCallKey draw_call_key;
};

void add_entity_to_rendering(ECS::EntityId entity_id);
void remove_entity_from_rendering(ECS::EntityId entity_id);

void queue_draw_calls(const RenderComponent *, DrawCallKey *, std::size_t count);
void sort_draw_calls(DrawCallKey *, std::size_t count);
void execute_draw_calls(const DrawCallKey *, std::size_t count);
} // namespace Age::Gfx
