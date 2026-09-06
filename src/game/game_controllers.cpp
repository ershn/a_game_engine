#include "main_loop.hpp"
#include "transformations.hpp"
#include "vector.hpp"

#include "game/game_controllers.hpp"

namespace Game
{
using namespace Age;

void control_game_via_keyboard(const GameKeyboardController &)
{
    if (Input::is_key_down(GLFW_KEY_ESCAPE))
        Core::request_exit();
}

void control_transform_via_keyboard(TransformKeyboardController &controller, Core::Transform &transform)
{
    using namespace Math;
    using Controller = TransformKeyboardController;

    if (Input::is_key_pressed(GLFW_KEY_F, controller.pressed_keys))
        controller.mode = static_cast<Controller::Mode>((controller.mode + 1U) % Controller::LAST_VALUE);

    switch (controller.mode)
    {
    case Controller::TRANSLATION:
        if (Input::is_key_down(GLFW_KEY_W))
            transform.position += forward<Vector3> * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_S))
            transform.position += backward<Vector3> * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_A))
            transform.position += left<Vector3> * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_D))
            transform.position += right<Vector3> * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_E))
            transform.position += up<Vector3> * controller.translation_speed;
        if (Input::is_key_down(GLFW_KEY_Q))
            transform.position += down<Vector3> * controller.translation_speed;
        break;
    case Controller::ORIENTATION:
        if (Input::is_key_down(GLFW_KEY_W))
            transform.orientation =
                axis_angle_quaternion(right<Vector3>, -controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_S))
            transform.orientation =
                axis_angle_quaternion(right<Vector3>, controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_A))
            transform.orientation =
                axis_angle_quaternion(up<Vector3>, -controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_D))
            transform.orientation =
                axis_angle_quaternion(up<Vector3>, controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_E))
            transform.orientation =
                axis_angle_quaternion(backward<Vector3>, controller.rotation_speed) * transform.orientation;
        if (Input::is_key_down(GLFW_KEY_Q))
            transform.orientation =
                axis_angle_quaternion(backward<Vector3>, -controller.rotation_speed) * transform.orientation;
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
