#pragma once

#include "ECS.hpp"
#include "GLFW.hpp"
#include "Vector.hpp"

namespace Age::Input
{
struct MouseInput
{
    static constexpr auto TYPE{Core::ComponentType::MOUSE_INPUT};

    float motion_sensitivity{0.01f};
    float scroll_sensitivity{1.0f};
};

bool is_key_pressed(int key);

Math::Vector2 get_cursor_position_delta(const MouseInput &mouse_input);
Math::Vector2 get_scroll_delta(const MouseInput &mouse_input);

bool is_exit_requested();

void init_input_system(GLFWwindow *window);

void update_input_state();
} // namespace Age::Input
