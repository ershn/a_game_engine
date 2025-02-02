#pragma once

#include <cstdint>

namespace Age::Core
{
enum class ComponentType : std::uint16_t
{
    TRANSFORM,
    LOCAL_TO_VIEW_MATRIX,
    LOCAL_TO_VIEW_NORMAL_MATRIX,

    MATERIAL,

    MODEL,

    UNIFORM_BUFFER_RANGE_BIND,
    UNIFORM_BUFFER_BLOCK,

    WORLD_TO_VIEW_MATRIX,
    VIEW_TO_CLIP_MATRIX,
    CAMERA,
    PROJECTION_BUFFER_BLOCK,

    POINT_LIGHT,
    LIGHT_DATA_BUFFER_BLOCK,

    RENDERER,

    SPHERICAL_CAMERA,

    MOUSE_INPUT,

    PATH_FOLLOWER,

    LAST_VALUE
};
} // namespace Age::Core
