#pragma once

#include <iostream>

#include "ecs.hpp"
#include "matrix.hpp"
#include "quaternion.hpp"
#include "vector.hpp"

namespace Age::Core
{
struct Transform
{
    static constexpr auto TYPE{ComponentType::TRANSFORM};

    Math::Vector3 position{};
    Math::Quaternion orientation{1.0f, 0.0f, 0.0f, 0.0f};
    Math::Vector3 scale{1.0f};
};

std::ostream &operator<<(std::ostream &out, const Transform &transform);

Math::Matrix4 transform_matrix(const Transform &transform);
} // namespace Age::Core
