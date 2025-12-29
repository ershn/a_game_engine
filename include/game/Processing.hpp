#pragma once

#include <cstdint>

#include "Input.hpp"
#include "Transform.hpp"

#include "game/Components.hpp"

namespace Game
{
struct GameKeyboardController
{
    static constexpr Age::Core::ComponentType TYPE{ComponentType::GAME_KEYBOARD_CONTROLLER};
};

void control_game_via_keyboard(const GameKeyboardController &);

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
