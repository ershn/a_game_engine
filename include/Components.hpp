#pragma once

#include <cstdint>

#include "Quaternion.hpp"
#include "Vector.hpp"

namespace Age::ECS
{
enum class ComponentType : std::uint16_t
{
    TRANSFORM,
    MODEL_TO_CAMERA_MATRIX,
    MATERIAL_PROPERTIES,
    SHADER,
    MODEL,
    RENDER,

    // Last value
    COUNT
};

constexpr std::size_t COMPONENT_TYPE_COUNT{static_cast<std::size_t>(ComponentType::COUNT)};

struct TransformComponent
{
    static constexpr ComponentType TYPE{ComponentType::TRANSFORM};

    Math::Vector3 position{};
    Math::Quaternion orientation{};
    Math::Vector3 scale{};
};
} // namespace Age::ECS
