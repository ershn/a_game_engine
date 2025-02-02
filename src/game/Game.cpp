#include <vector>

#include "ECS.hpp"
#include "Rendering.hpp"
#include "Transformations.hpp"

#include "MaterialInstances.hpp"
#include "MeshInstances.hpp"
#include "Path.hpp"
#include "ShaderInstances.hpp"
#include "SphericalCamera.hpp"
#include "UniformBlocks.hpp"

#include "game/Game.hpp"
#include "game/GroundMesh.hpp"
#include "game/Processing.hpp"
#include "game/ShadersAndMaterials.hpp"

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

void init_entities()
{
    Gfx::ModelId next_model_id{Gfx::USER_MODEL_START_ID};
    Gfx::UniformBufferId next_uniform_buffer_id{0};

    auto ground_model_id = next_model_id++;
    Gfx::create_model<1>(ground_model_id, create_ground_mesh);

    Gfx::create_shader<Gfx::NoLightingShader>(NO_LIGHTING_SHADER, "shaders/no_lighting.vert",
                                              "shaders/no_lighting.frag");
    Gfx::create_shader<Gfx::NoLightingColorShader>(NO_LIGHTING_COLOR_SHADER, "shaders/no_lighting_color.vert",
                                                   "shaders/no_lighting.frag");
    Gfx::create_shader<FragmentLightingShader>(FRAGMENT_LIGHTING_SHADER, "shaders/fragment_lighting.vert",
                                               "shaders/fragment_lighting.frag");
    Gfx::create_shader<FragmentLightingColorShader>(
        FRAGMENT_LIGHTING_COLOR_SHADER, "shaders/fragment_lighting_color.vert", "shaders/fragment_lighting.frag");

    Gfx::create_material<Gfx::NoLightingMaterial>(NO_LIGHTING_MATERIAL, NO_LIGHTING_SHADER);

    auto projection_buffer_id = next_uniform_buffer_id++;
    auto &projection_buffer =
        Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionUniformBlock>>(projection_buffer_id);
    auto light_data_buffer_id = next_uniform_buffer_id++;
    auto &light_data_buffer =
        Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::LightDataUniformBlock>>(light_data_buffer_id);

    auto material_buffer_id = next_uniform_buffer_id++;
    auto &material_buffer =
        Gfx::create_uniform_buffer<Gfx::ArrayUniformBuffer<Gfx::MaterialUniformBlock>>(material_buffer_id, 4);
    auto material_buffer_writer = Gfx::ArrayUniformBufferWriter{material_buffer};

    Gfx::MaterialId next_material_id{100};

    // Light path
    std::vector<Math::Vector3> light_path{{13.5000f, 3.00000f, 9.80000f},   {5.99999f, 3.00000f, 18.1000f},
                                          {-14.1000f, 4.70000f, 9.70000f},  {-4.30000f, 8.80000f, -0.400000f},
                                          {-14.5000f, 5.20000f, -9.60000f}, {-4.90000f, 5.20000f, -12.6000f},
                                          {13.7000f, 5.20000f, -12.6000f}};

    // Camera
    {
        Gfx::Camera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};
        Gfx::WorldToViewMatrix view_matrix{};
        Gfx::ViewToClipMatrix clip_matrix{
            Math::perspective_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)};
        Gfx::ProjectionBufferBlock camera_projection{projection_buffer.get_block()};
        Gfx::LightDataBufferBlock camera_light{light_data_buffer.get_block()};
        Input::MouseInput camera_input{.motion_sensitivity{0.005f}};
        Gfx::SphericalCamera spherical_camera{.origin{0.0f, 2.0f, 0.0f},
                                              .spherical_coord{Math::Vector2{Math::radians(60.0f), 0.0f}, 30.0f}};
        spherical_camera.cartesian_coord = Math::to_cartesian_coord(spherical_camera.spherical_coord);

        Core::create_entity(camera, view_matrix, clip_matrix, camera_projection, camera_light, camera_input,
                            spherical_camera, GameKeyboardController{});
    }

    // Light
    {
        auto material_id = next_material_id++;
        Gfx::create_material<Gfx::NoLightingColorMaterial>(material_id, NO_LIGHTING_COLOR_SHADER);

        auto id = Core::create_entity(
            Core::Transform{.position{10.0f, 3.0f, 1.0f}, .scale{0.2f}}, Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id}, Gfx::ModelRef{Gfx::CUBE_MODEL_ID}, Gfx::Renderer{},
            Gfx::PointLight{.light_intensity{0.8f, 0.8f, 0.8f, 1.0f},
                            .light_attenuation{0.2f},
                            .ambient_light_intensity{0.2f, 0.2f, 0.2f, 1.0f}},
            Core::PathFollower{
                .path{light_path}, .target_position{light_path[0]}, .move_speed{3.0f}, .repeat_path{true}});

        Gfx::init_renderer(id);
    }

    // Floor
    {
        auto material_id = next_material_id++;
        Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);

        material_buffer_writer[0] = {.specular_color{1.0f}, .surface_shininess{0.95f}};

        auto id = Core::create_entity(
            Core::Transform{.orientation{Math::axis_angle_quaternion({1.0f, 0.0f, 0.0f}, Math::radians(-90.0f))},
                            .scale{0.2f}},
            Gfx::LocalToViewMatrix{}, Gfx::LocalToViewNormalMatrix{}, Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(0).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::ModelRef{ground_model_id}, Gfx::Renderer{});

        Gfx::init_renderer(id, Gfx::RENDER_WITH_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND);
    }

    // Cylinder
    {
        auto model_id = Gfx::CYLINDER_MODEL_ID;
        auto material_id = next_material_id++;
        auto &material =
            Gfx::create_material<FragmentLightingColorMaterial>(material_id, FRAGMENT_LIGHTING_COLOR_SHADER);
        material.color = {0.968f, 0.141f, 0.019f, 1.0f};

        material_buffer_writer[1] = {.specular_color{1.0f}, .surface_shininess{0.5f}};

        auto id = Core::create_entity(
            Core::Transform{.position{-0.300010f, 7.40000f, -2.30000f},
                            .orientation{1.00000f, 0.00000f, 0.00000f, 0.00000f},
                            .scale{4.00000f, 4.00000f, 4.00000f}},
            Gfx::LocalToViewMatrix{}, Gfx::LocalToViewNormalMatrix{}, Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(1).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::ModelRef{model_id}, Gfx::Renderer{});

        Gfx::init_renderer(id, Gfx::RENDER_WITH_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND);
    }

    // Cube 1
    {
        auto model_id = Gfx::CUBE_MODEL_ID;
        auto material_id = next_material_id++;
        Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);

        material_buffer_writer[2] = {.specular_color{1.0f}, .surface_shininess{0.5f}};

        auto id = Core::create_entity(
            Core::Transform{.position{12.9000f, 3.40000f, 17.6000f},
                            .orientation{0.859259f, -0.268912f, -0.229626f, -0.369666f},
                            .scale{4.40000f, 4.40000f, 4.40000f}},
            Gfx::LocalToViewMatrix{}, Gfx::LocalToViewNormalMatrix{}, Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(2).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::ModelRef{model_id}, Gfx::Renderer{});

        Gfx::init_renderer(id, Gfx::RENDER_WITH_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND);
    }

    // Cube 2
    {
        auto model_id = Gfx::CUBE_MODEL_ID;
        auto material_id = next_material_id++;
        Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);

        material_buffer_writer[3] = {.specular_color{1.0f}, .surface_shininess{0.5f}};

        auto id = Core::create_entity(
            Core::Transform{.position{-16.0000f, 7.90000f, -14.2000f},
                            .orientation{0.594317f, -0.573115f, -0.382432f, -0.414812f},
                            .scale{4.50000f, 4.40000f, 13.1000f}},
            Gfx::LocalToViewMatrix{}, Gfx::LocalToViewNormalMatrix{}, Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(3).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::ModelRef{model_id}, Gfx::Renderer{});

        Gfx::init_renderer(id, Gfx::RENDER_WITH_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND);
    }

    material_buffer_writer.apply();
}

void run_systems()
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(control_material_via_keyboard);
    process_components(control_transform_via_keyboard);
    process_components(Gfx::update_spherical_camera_via_input);
    process_components(Gfx::calc_spherical_camera_view_matrix);
    process_components(Core::move_along_path);
}
} // namespace Game
