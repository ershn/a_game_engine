#include <algorithm>
#include <cmath>

#include "Camera.hpp"
#include "DefaultMaterials.hpp"
#include "Input.hpp"
#include "MainLoop.hpp"
#include "Math.hpp"
#include "OpenGL.hpp"
#include "Time.hpp"
#include "Transformations.hpp"

#include "game/Meshes.hpp"
#include "game/Processing.hpp"
#include "game/ShadersAndMaterials.hpp"

namespace Game
{
using namespace Age;

void control_game_via_keyboard(const GameKeyboardController &)
{
    if (Input::is_key_down(GLFW_KEY_ESCAPE))
        Core::exit();
}

void control_material_via_keyboard(const MaterialKeyboardController &, const Gfx::MaterialRef &material_ref)
{
    auto &material = static_cast<Gfx::FragmentLightingColorMaterial &>(Gfx::get_material(material_ref.material_id));

    if (Input::is_key_down(GLFW_KEY_B))
        material.surface_shininess = std::clamp(material.surface_shininess + 0.01f, 0.01f, 1.0f);
    else if (Input::is_key_down(GLFW_KEY_G))
        material.surface_shininess = std::clamp(material.surface_shininess - 0.01f, 0.01f, 1.0f);
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

void update_sunlight(Sunlight &sunlight, Core::Transform &transform, Gfx::DirectionalLight &directional_light)
{
    sunlight.time += Time::delta_time();
    if (sunlight.time >= sunlight.day_length)
        sunlight.time -= sunlight.day_length;

    float normalized_time{sunlight.time / sunlight.day_length};
    auto intensity_it_2 = std::find_if(
        sunlight.light_intensities.cbegin(), sunlight.light_intensities.cend(), [=](const auto &light_intensity) {
            return normalized_time < light_intensity.normalized_time;
        }
    );
    auto intensity_it_1 = intensity_it_2 - 1;

    float segment_normalized_time{
        Math::inverse_lerp(intensity_it_1->normalized_time, intensity_it_2->normalized_time, normalized_time)
    };
    directional_light.light_intensity =
        Math::lerp(intensity_it_1->intensity, intensity_it_2->intensity, segment_normalized_time);

    float sun_angle{Math::TAU - Math::TAU * normalized_time - Math::PI * 0.5f};
    transform.position = {std::cos(sun_angle), std::sin(sun_angle), 0.0f};

    auto &camera_render_state = Core::get_entity_component<Gfx::CameraRenderState>(sunlight.camera_id);
    camera_render_state.clear_color =
        Math::lerp(intensity_it_1->sky_color, intensity_it_2->sky_color, segment_normalized_time);

    auto &light_settings = Core::get_entity_component<Gfx::GlobalLightSettings>(sunlight.light_settings_id);
    light_settings.max_intensity =
        Math::lerp(intensity_it_1->max_intensity, intensity_it_2->max_intensity, segment_normalized_time);
    light_settings.ambient_light_intensity =
        Math::lerp(intensity_it_1->ambient_intensity, intensity_it_2->ambient_intensity, segment_normalized_time);
}

void update_sphere_impostors(const SphereImpostors &sphere_impostors, const Gfx::MeshRef &mesh_ref)
{
    auto &wv_matrix = Core::get_entity_component<const Gfx::WorldToViewMatrix>(sphere_impostors.camera_id).matrix;
    auto vbo = Gfx::get_mesh_buffers(mesh_ref.mesh_id).vertex_buffer_object;

    SphereImpostor buffer_values[4];
    for (std::size_t index{}; index < sphere_impostors.instance_count; ++index)
    {
        auto &instance = sphere_impostors.instances[index];
        buffer_values[index] = {
            .viewPosition{Math::Vector3{wv_matrix * Math::Vector4{instance.worldPosition, 1.0f}}},
            .radius{instance.radius}
        };
    }

    Gfx::OGL::write_array_buffer(vbo, buffer_values, sphere_impostors.instance_count * sizeof(SphereImpostor));
}
} // namespace Game
