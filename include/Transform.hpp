#pragma once

#include "ECS.hpp"
#include "Quaternion.hpp"
#include "Vector.hpp"

namespace Age::Core
{
struct Transform
{
    static constexpr auto TYPE{ComponentType::TRANSFORM};

    Math::Vector3 position{};
    Math::Quaternion orientation{1.0f, 0.0f, 0.0f, 0.0f};
    Math::Vector3 scale{1.0f};
};
} // namespace Age::Core
