#pragma once

#include <cstdint>
#include <type_traits>

#include "Components.hpp"
#include "Material.hpp"
#include "Transform.hpp"

namespace Game
{
enum ComponentType : std::underlying_type_t<Age::Core::ComponentType>
{
    GAME_KEYBOARD_CONTROLLER = Age::Core::ComponentType::LAST_VALUE,
    MATERIAL_KEYBOARD_CONTROLLER,
    TRANSFORM_KEYBOARD_CONTROLLER,

    LAST_VALUE
};

struct GameKeyboardController
{
    static constexpr Age::Core::ComponentType TYPE{ComponentType::GAME_KEYBOARD_CONTROLLER};
};

void control_game_via_keyboard(const GameKeyboardController &);

struct MaterialKeyboardController
{
    static constexpr Age::Core::ComponentType TYPE{ComponentType::MATERIAL_KEYBOARD_CONTROLLER};
};

void control_material_via_keyboard(const MaterialKeyboardController &, const Age::Gfx::MaterialRef &material_ref);

struct TransformKeyboardController
{
    static constexpr Age::Core::ComponentType TYPE{ComponentType::TRANSFORM_KEYBOARD_CONTROLLER};

    enum Mode : std::uint8_t
    {
        TRANSLATION,
        ORIENTATION,
        SCALING,

        LAST_VALUE
    };

    Age::Input::PressedKeys pressed_keys{};
    float translation_speed{0.1f};
    float rotation_speed{0.05f};
    float scaling_speed{0.05f};
    Mode mode{};
};

void control_transform_via_keyboard(TransformKeyboardController &controller, Age::Core::Transform &transform);
} // namespace Game
