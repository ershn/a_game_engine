#pragma once

#include <type_traits>

#include "Components.hpp"
#include "Material.hpp"

namespace Game
{
enum ComponentType : std::underlying_type_t<Age::Core::ComponentType>
{
    CAMERA_KEYBOARD_CONTROLLER = Age::Core::ComponentType::LAST_VALUE,
    MATERIAL_KEYBOARD_CONTROLLER,

    LAST_VALUE
};

struct CameraKeyboardController
{
    static constexpr Age::Core::ComponentType TYPE{ComponentType::CAMERA_KEYBOARD_CONTROLLER};
};

void process_camera_keyboard_input(const CameraKeyboardController &keyboard_controller);

struct MaterialKeyboardController
{
    static constexpr Age::Core::ComponentType TYPE{ComponentType::MATERIAL_KEYBOARD_CONTROLLER};
};

void process_material_keyboard_input(const MaterialKeyboardController &material_controller,
                                     const Age::Gfx::MaterialRef &material_ref);
} // namespace Game
