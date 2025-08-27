#pragma once

#include <type_traits>

#include "../Components.hpp"

namespace Game
{
enum ComponentType : std::underlying_type_t<Age::Core::ComponentType>
{
    GAME_KEYBOARD_CONTROLLER = Age::Core::ComponentType::LAST_VALUE,
    MATERIAL_KEYBOARD_CONTROLLER,
    TRANSFORM_KEYBOARD_CONTROLLER,
    SPHERICAL_CAMERA_MOUSE_CONTROLLER,

    SUNLIGHT,

    SPHERE_IMPOSTOR_MATERIALS_BUFFER_BLOCK,
    SPHERE_IMPOSTORS,

    INFINITY_SYMBOL,
    CHECKERBOARD_SCENE_CONTROLLER,
    SPHERICAL_CAMERA_VIEW_MATRIX_UPDATER,
    VIEW_MATRIX_READER,
    POST_PROJECTION_ROTATION,

    LAST_VALUE
};
} // namespace Game
