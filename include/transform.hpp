#pragma once

#include <iostream>

#include "components.hpp"
#include "matrix.hpp"
#include "quaternion.hpp"
#include "vector.hpp"

namespace Age::Core
{
/*
 * Single hierarchy in parent-child order with possible holes (heap)
 * r1
 *   c1
 *   c2
 *     d1
 *     d2
 *   c3
 *
 * r2
 * r3
 *   c1
 *     d1
 *
 * Reading:
 *   hierarchy index of the transform stored in the component counterpart
 *   access the transform through the component index each time needed
 * Reparenting:
 *   3 types of operation: add, remove, move (remove then add)
 *   store add and remove operations in separate lists and in parent-child order
 *   perform the reparenting at the end of the frame: remove first, add second
 *   perform the remove operations in reverse order, the add operations in order
 *   use the EntityIds to specify the parent-child relationships in the API
 *   use the EntityId returned on entity creation to parent to a newly created entity during the same frame
 *   use separate ids to express parent-child relationships in assets, convert the ids to EntityIds on instantiation
 * Transforming:
 *   update the dirty transforms in parent-child order
 *   use parent local-world matrices to calculate children local-world matrices
 */

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
