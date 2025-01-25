#pragma once

#include <cstdint>

namespace Age::Core
{
enum class ComponentType : std::uint16_t
{
    TRANSFORM,
    MODEL_TO_CAMERA_MATRIX,
    MODEL_TO_CAMERA_NORMAL_MATRIX,

    MATERIAL,

    MODEL,

    WORLD_TO_CAMERA_MATRIX,
    CAMERA_TO_CLIP_MATRIX,
    CAMERA,
    PROJECTION_BUFFER,

    POINT_LIGHT,
    LIGHT_DATA_BUFFER_REF,

    RENDERER,

    SPHERICAL_CAMERA,

    MOUSE_INPUT,

    LAST_VALUE
};
} // namespace Age::Core
