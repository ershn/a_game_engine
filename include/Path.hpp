#pragma once

#include <cstdint>
#include <vector>

#include "Components.hpp"
#include "Transform.hpp"
#include "Vector.hpp"

namespace Age::Core
{
struct PathFollower
{
    static constexpr auto TYPE{ComponentType::PATH_FOLLOWER};

    std::vector<Math::Vector3> path{};
    std::uint32_t target_index{};
    Math::Vector3 target_position{};
    float target_min_distance{0.1f};
    float move_speed{1.0f};
    bool repeat_path{};
};

void move_along_path(Transform &transform, PathFollower &path_follower);
} // namespace Age::Core
