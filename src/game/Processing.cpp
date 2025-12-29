#include "MainLoop.hpp"
#include "Transformations.hpp"
#include "Vector.hpp"

#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

void control_game_via_keyboard(const GameKeyboardController &)
{
    if (Input::is_key_down(GLFW_KEY_ESCAPE))
        Core::exit();
}

void control_transform_via_keyboard(TransformKeyboardController &controller, Core::Transform &transform)
{
    using Math::Vector3;
    using Controller = TransformKeyboardController;

    if (Input::is_key_pressed(GLFW_KEY_F, controller.pressed_keys))
        controller.mode = static_cast<Controller::Mode>((controller.mode + 1U) % Controller::LAST_VALUE);

    switch (controller.mode)
    {
    case Controller::TRANSLATION:
        if (Input::is_key_down(GLFW_KEY_W))
            transform.position += Vector3::forward * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_S))
            transform.position += Vector3::backward * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_A))
            transform.position += Vector3::left * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_D))
            transform.position += Vector3::right * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_E))
            transform.position += Vector3::up * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_Q))
            transform.position += Vector3::down * controller.translation_speed;
        break;
    case Controller::ORIENTATION:
        if (Input::is_key_down(GLFW_KEY_W))
            transform.orientation =
                Math::axis_angle_quaternion(Vector3::right, -controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_S))
            transform.orientation =
                Math::axis_angle_quaternion(Vector3::right, controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_A))
            transform.orientation =
                Math::axis_angle_quaternion(Vector3::up, -controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_D))
            transform.orientation =
                Math::axis_angle_quaternion(Vector3::up, controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_E))
            transform.orientation =
                Math::axis_angle_quaternion(Vector3::backward, controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_Q))
            transform.orientation =
                Math::axis_angle_quaternion(Vector3::backward, -controller.rotation_speed) * transform.orientation;
        break;
    case Controller::SCALING:
        if (Input::is_key_down(GLFW_KEY_W))
            transform.scale += Vector3{0.0f, 0.0f, 1.0f} * controller.scaling_speed;
        if (Input::is_key_down(GLFW_KEY_S))
            transform.scale += Vector3{0.0f, 0.0f, -1.0f} * controller.scaling_speed;
        if (Input::is_key_down(GLFW_KEY_A))
            transform.scale += Vector3{-1.0f, 0.0f, 0.0f} * controller.scaling_speed;
        if (Input::is_key_down(GLFW_KEY_D))
            transform.scale += Vector3{1.0f, 0.0f, 0.0f} * controller.scaling_speed;
        if (Input::is_key_down(GLFW_KEY_E))
            transform.scale += Vector3{0.0f, 1.0f, 0.0f} * controller.scaling_speed;
        if (Input::is_key_down(GLFW_KEY_Q))
            transform.scale += Vector3{0.0f, -1.0f, 0.0f} * controller.scaling_speed;
        break;
    }

    if (Input::is_key_pressed(GLFW_KEY_SPACE, controller.pressed_keys))
        std::cout << transform << '\n';
}
} // namespace Game
