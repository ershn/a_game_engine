#include <bitset>

#include "Input.hpp"

namespace Age::Input
{
namespace
{
GLFWwindow *s_window{};

double s_prev_cursor_position_x{};
double s_prev_cursor_position_y{};
double s_cursor_position_x{};
double s_cursor_position_y{};
Math::Vector2 s_cursor_position_delta{};

double s_scroll_delta_x{};
double s_scroll_delta_y{};

void scroll_callback(GLFWwindow *window, double x_delta, double y_delta)
{
    s_scroll_delta_x += x_delta;
    s_scroll_delta_y += y_delta;
}

Math::Vector2 s_scroll_delta{};
} // namespace

bool is_key_up(int key)
{
    return glfwGetKey(s_window, key) == GLFW_RELEASE;
}

bool is_key_down(int key)
{
    return glfwGetKey(s_window, key) == GLFW_PRESS;
}

bool is_key_pressed(int key, PressedKeys &pressed_keys)
{
    bool is_key_pressed{glfwGetKey(s_window, key) == GLFW_PRESS};
    auto was_key_pressed{pressed_keys[key]};
    if (is_key_pressed && was_key_pressed)
        return false;
    was_key_pressed = is_key_pressed;
    return is_key_pressed;
}

Math::Vector2 get_cursor_position_delta(const MouseInput &mouse_input)
{
    return s_cursor_position_delta * mouse_input.motion_sensitivity;
}

Math::Vector2 get_scroll_delta(const MouseInput &mouse_input)
{
    return s_scroll_delta * mouse_input.scroll_sensitivity;
}

bool is_exit_requested()
{
    return glfwWindowShouldClose(s_window);
}

void init_input_system(GLFWwindow *window)
{
    s_window = window;

    glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetScrollCallback(s_window, scroll_callback);

    glfwPollEvents();

    glfwGetCursorPos(s_window, &s_cursor_position_x, &s_cursor_position_y);
    s_prev_cursor_position_x = s_cursor_position_x;
    s_prev_cursor_position_y = s_cursor_position_y;
}

void update_input_state()
{
    glfwPollEvents();

    glfwGetCursorPos(s_window, &s_cursor_position_x, &s_cursor_position_y);
    s_cursor_position_delta.x = static_cast<float>(s_cursor_position_x - s_prev_cursor_position_x);
    s_cursor_position_delta.y = static_cast<float>(s_cursor_position_y - s_prev_cursor_position_y);
    s_prev_cursor_position_x = s_cursor_position_x;
    s_prev_cursor_position_y = s_cursor_position_y;

    s_scroll_delta.x = static_cast<float>(s_scroll_delta_x);
    s_scroll_delta.y = static_cast<float>(s_scroll_delta_y);
    s_scroll_delta_x = 0.0;
    s_scroll_delta_y = 0.0;
}
} // namespace Age::Input
