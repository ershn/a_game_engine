#include "Input.hpp"
#include "MainLoop.hpp"
#include "MaterialInstances.hpp"

#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

void process_camera_keyboard_input(const CameraKeyboardController &keyboard_controller)
{
    if (Input::is_key_pressed(GLFW_KEY_ESCAPE))
        Core::exit();
}

void process_material_keyboard_input(const MaterialKeyboardController &material_controller,
                                     const Age::Gfx::MaterialRef &material_ref)
{
    auto &material = static_cast<Gfx::FragmentLightingColorMaterial &>(Gfx::get_material(material_ref.material_id));

    if (Input::is_key_pressed(GLFW_KEY_B))
        material.surface_shininess = std::clamp(material.surface_shininess + 0.01f, 0.01f, 1.0f);
    else if (Input::is_key_pressed(GLFW_KEY_G))
        material.surface_shininess = std::clamp(material.surface_shininess - 0.01f, 0.01f, 1.0f);
}
} // namespace Game
