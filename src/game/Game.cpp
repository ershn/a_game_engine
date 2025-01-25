#include "game/Game.hpp"

#include "ECS.hpp"
#include "Rendering.hpp"
#include "Transformations.hpp"

#include "MaterialInstances.hpp"
#include "MeshInstances.hpp"
#include "ShaderInstances.hpp"
#include "SphericalCamera.hpp"
#include "UniformBufferInstances.hpp"

#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

std::size_t get_component_type_count()
{
    return ComponentType::LAST_VALUE - static_cast<std::size_t>(Core::ComponentType::LAST_VALUE);
}

constexpr Gfx::ShaderId NO_LIGHTING_SHADER{0};
constexpr Gfx::ShaderId NO_LIGHTING_COLOR_SHADER{1};
constexpr Gfx::ShaderId FRAGMENT_LIGHTING_SHADER{2};
constexpr Gfx::ShaderId FRAGMENT_LIGHTING_COLOR_SHADER{3};

constexpr Gfx::MaterialId NO_LIGHTING_MATERIAL{0};

constexpr Gfx::UniformBufferId PROJECTION_UNIFORM_BUFFER{0};
constexpr Gfx::UniformBufferId LIGHT_DATA_UNIFORM_BUFFER{1};

void init_entities()
{
    Gfx::create_shader<Gfx::NoLightingShader>(NO_LIGHTING_SHADER, "shaders/no_lighting.vert",
                                              "shaders/no_lighting.frag");
    Gfx::create_shader<Gfx::NoLightingColorShader>(NO_LIGHTING_COLOR_SHADER, "shaders/no_lighting_color.vert",
                                                   "shaders/no_lighting.frag");
    Gfx::create_shader<Gfx::FragmentLightingShader>(FRAGMENT_LIGHTING_SHADER, "shaders/fragment_lighting.vert",
                                                    "shaders/fragment_lighting.frag");
    Gfx::create_shader<Gfx::FragmentLightingColorShader>(
        FRAGMENT_LIGHTING_COLOR_SHADER, "shaders/fragment_lighting_color.vert", "shaders/fragment_lighting.frag");

    Gfx::create_material<Gfx::NoLightingMaterial>(NO_LIGHTING_MATERIAL, NO_LIGHTING_SHADER);

    Gfx::create_uniform_buffer<Gfx::ProjectionUniformBuffer>(PROJECTION_UNIFORM_BUFFER, Gfx::PROJECTION_BLOCK_BINDING);
    Gfx::create_uniform_buffer<Gfx::LightDataUniformBuffer>(LIGHT_DATA_UNIFORM_BUFFER, Gfx::LIGHT_DATA_BLOCK_BINDING);

    Gfx::MaterialId next_material_id{100};

    // Camera
    {
        Gfx::Camera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};
        Gfx::WorldToCameraMatrix camera_matrix{};
        Gfx::CameraToClipMatrix clip_matrix{
            Math::perspective_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)};
        Gfx::ProjectionBufferRef camera_projection{PROJECTION_UNIFORM_BUFFER};
        Gfx::LightDataBufferRef camera_light{LIGHT_DATA_UNIFORM_BUFFER};
        Input::MouseInput camera_input{.motion_sensitivity{0.005f}};
        Gfx::SphericalCamera spherical_camera{.origin{0.0f, 1.0f, 0.0f},
                                              .spherical_coord{Math::Vector2{Math::radians(45.0f), 0.0f}, 5.0f}};
        spherical_camera.cartesian_coord = Math::to_cartesian_coord(spherical_camera.spherical_coord);

        Core::create_entity(camera, camera_matrix, clip_matrix, camera_projection, camera_light, camera_input,
                            spherical_camera, CameraKeyboardController{});
    }

    // Light
    {
        auto material_id = next_material_id++;
        Gfx::create_material<Gfx::NoLightingColorMaterial>(material_id, NO_LIGHTING_COLOR_SHADER);

        Core::Transform xform{.position{2.0f, 3.0f, 1.0f}, .scale{0.2f}};
        Gfx::ModelToCameraMatrix camera_matrix{};
        Gfx::ModelRef model{Gfx::CUBE_MODEL_ID};
        Gfx::PointLight point_light{.light_intensity{0.8f, 0.8f, 0.8f, 1.0f},
                                    .light_attenuation{0.2f},
                                    .ambient_light_intensity{0.2f, 0.2f, 0.2f, 1.0f}};

        auto id = Core::create_entity(xform, camera_matrix, Gfx::MaterialRef{material_id}, model, Gfx::Renderer{},
                                      point_light);

        auto [renderer, model_to_camera_matrix] =
            Core::get_entity_components<Gfx::Renderer, const Gfx::ModelToCameraMatrix>(id);
        Gfx::init_renderer(renderer, model_to_camera_matrix.matrix, material_id, model.model_id);
    }

    // Floor
    {
        auto model_id = Gfx::PLANE_MODEL_ID;
        auto material_id = next_material_id++;
        Gfx::create_material<Gfx::FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);

        auto id = Core::create_entity(
            Core::Transform{.orientation{Math::axis_angle_quaternion({1.0f, 0.0f, 0.0f}, Math::radians(-90.0f))},
                            .scale{100.0f}},
            Gfx::ModelToCameraMatrix{}, Gfx::ModelToCameraNormalMatrix{}, Gfx::MaterialRef{material_id},
            Gfx::ModelRef{}, Gfx::Renderer{});

        auto [renderer, model_to_camera_matrix, model_to_camera_normal_matrix] =
            Core::get_entity_components<Gfx::Renderer, const Gfx::ModelToCameraMatrix,
                                        const Gfx::ModelToCameraNormalMatrix>(id);
        Gfx::init_renderer(renderer, model_to_camera_matrix.matrix, model_to_camera_normal_matrix.matrix, material_id,
                           model_id);
    }

    // Center cube
    {
        auto model_id = Gfx::CYLINDER_MODEL_ID;
        auto material_id = next_material_id++;
        auto &material =
            Gfx::create_material<Gfx::FragmentLightingColorMaterial>(material_id, FRAGMENT_LIGHTING_COLOR_SHADER);
        material.color = {0.968f, 0.141f, 0.019f, 1.0f};

        auto id = Core::create_entity(Core::Transform{.position{0.0f, 1.0f, 0.0f}}, Gfx::ModelToCameraMatrix{},
                                      Gfx::ModelToCameraNormalMatrix{}, Gfx::MaterialRef{material_id},
                                      Gfx::ModelRef{model_id}, Gfx::Renderer{}, MaterialKeyboardController{});

        auto [renderer, model_to_camera_matrix, model_to_camera_normal_matrix] =
            Core::get_entity_components<Gfx::Renderer, const Gfx::ModelToCameraMatrix,
                                        const Gfx::ModelToCameraNormalMatrix>(id);
        Gfx::init_renderer(renderer, model_to_camera_matrix.matrix, model_to_camera_normal_matrix.matrix, material_id,
                           model_id);
    }
}

void run_systems()
{
    Core::process_components(process_camera_keyboard_input);
    Core::process_components(process_material_keyboard_input);
    Core::process_components(Gfx::update_spherical_camera_via_input);
    Core::process_components(Gfx::calc_spherical_camera_matrix);
}
} // namespace Game
