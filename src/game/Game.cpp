#include <vector>

#include "ECS.hpp"
#include "Rendering.hpp"
#include "Transformations.hpp"

#include "Color.hpp"
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
    Gfx::UniformBufferId next_uniform_buffer_id{0};
    Gfx::MaterialId next_material_id{100};
    Gfx::ModelId next_model_id{Gfx::USER_MODEL_START_ID};

    auto ground_model_id = next_model_id++;
    Gfx::create_model<1>(ground_model_id, create_ground_mesh);

    Gfx::create_shader<Gfx::NoLightingShader>(
        NO_LIGHTING_SHADER, "shaders/no_lighting.vert", "shaders/no_lighting.frag"
    );
    Gfx::create_shader<Gfx::NoLightingColorShader>(
        NO_LIGHTING_COLOR_SHADER, "shaders/no_lighting_color.vert", "shaders/no_lighting.frag"
    );
    Gfx::create_shader<FragmentLightingShader>(
        FRAGMENT_LIGHTING_SHADER, "shaders/fragment_lighting.vert", "shaders/fragment_lighting.frag"
    );
    Gfx::create_shader<FragmentLightingColorShader>(
        FRAGMENT_LIGHTING_COLOR_SHADER, "shaders/fragment_lighting_color.vert", "shaders/fragment_lighting.frag"
    );

    auto &no_lighting_material =
        Gfx::create_material<Gfx::NoLightingMaterial>(NO_LIGHTING_MATERIAL, NO_LIGHTING_SHADER);

    auto gamma_correction_buffer_id = next_uniform_buffer_id++;
    auto &gamma_correction_buffer =
        Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::GammaCorrectionUniformBlock>>(
            gamma_correction_buffer_id
        );

    auto projection_buffer_id = next_uniform_buffer_id++;
    auto &projection_buffer =
        Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionUniformBlock>>(projection_buffer_id);

    auto lights_buffer_id = next_uniform_buffer_id++;
    auto &lights_buffer =
        Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::LightsUniformBlock>>(lights_buffer_id);

    auto material_buffer_id = next_uniform_buffer_id++;
    auto &material_buffer =
        Gfx::create_uniform_buffer<Gfx::ArrayUniformBuffer<Gfx::MaterialUniformBlock>>(material_buffer_id, 4);
    auto material_buffer_writer = Gfx::ArrayUniformBufferWriter{material_buffer};

    // Light data
    std::vector<Math::Vector3> point_light_path_1{
        {13.5000f, 3.00000f, 9.80000f},
        {5.99999f, 3.00000f, 18.1000f},
        {-14.1000f, 4.70000f, 9.70000f},
        {-4.30000f, 8.80000f, -0.400000f},
        {-14.5000f, 5.20000f, -9.60000f},
        {-4.90000f, 5.20000f, -12.6000f},
        {13.7000f, 5.20000f, -12.6000f}
    };

    std::vector<Math::Vector3> point_light_path_2{
        {-0.600002f, 6.20000f, 0.700000f},
        {-4.30000f, 3.30000f, 6.00000f},
        {-16.7000f, 3.90000f, 7.30000f},
        {-16.7000f, 3.90000f, 2.90000f},
        {-15.7000f, 10.2000f, -11.0000f},
        {-2.30000f, 2.90000f, -9.10000f},
        {-2.04146e-06f, 6.90000f, -6.40000f},
        {-0.500002f, 11.1000f, -5.80000f},
        {-0.500002f, 11.1000f, 1.30000f}
    };

    std::vector<Math::Vector3> point_light_path_3{
        {-5.00000f, -3.30000f, 18.3000f},
        {1.40000f, -3.30000f, 12.3000f},
        {8.59999f, -3.30000f, 5.10000f},
        {10.2000f, -3.30000f, -2.00000f},
        {9.50000f, -3.30000f, -9.20000f},
        {4.00000f, -3.30000f, -15.1000f},
        {7.69999f, 2.20000f, -15.1000f},
        {11.7000f, 3.80000f, -15.1000f},
        {16.5000f, 2.10000f, -11.9000f},
        {15.6000f, 2.10000f, -1.30000f},
        {13.4000f, 2.10000f, 8.50000f},
        {10.5000f, 2.10000f, 13.5000f},
        {5.00000f, 2.10000f, 20.4000f},
        {0.899998f, 2.10000f, 19.0000f}
    };

    std::vector<Sunlight::LightIntensity> sunlight_intensities{
        {0.0f, 3.0f, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f}},
        {0.2f, 3.0f, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f}},
        {0.25f,
         5.0f,
         {2.5f, 0.2f, 0.2f, 1.0f},
         {0.01f, 0.025f, 0.025f, 1.0f},
         {115 / 255.0f, 215 / 255.0f, 255 / 255.0f, 1.0f}},
        {0.3f,
         10.0f,
         {6.5f, 6.5f, 6.5f, 1.0f},
         {0.4f, 0.4f, 0.4f, 1.0f},
         {185 / 255.0f, 235 / 255.0f, 255 / 255.0f, 1.0f}},
        {0.7f,
         10.0f,
         {6.5f, 6.5f, 6.5f, 1.0f},
         {0.4f, 0.4f, 0.4f, 1.0f},
         {185 / 255.0f, 235 / 255.0f, 255 / 255.0f, 1.0f}},
        {0.75f,
         5.0f,
         {2.5f, 0.2f, 0.2f, 1.0f},
         {0.01f, 0.025f, 0.025f, 1.0f},
         {115 / 255.0f, 215 / 255.0f, 255 / 255.0f, 1.0f}},
        {0.8f, 3.0f, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f}},
        {1.0f, 3.0f, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f}}
    };

    // Camera
    Core::EntityId camera_id;
    {
        Gfx::Camera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{},
            Gfx::ViewToClipMatrix{
                Math::perspective_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::RenderState{.clear_color{0.294f, 0.22f, 0.192f, 1.0f}},
            Gfx::GammaCorrectionBufferBlock{gamma_correction_buffer.get_block()},
            Gfx::ProjectionBufferBlock{projection_buffer.get_block()},
            Gfx::LightsBufferBlock{lights_buffer.get_block()},
            Input::MouseInput{.motion_sensitivity{0.005f}},
            Gfx::SphericalCamera{
                .origin{0.0f, 2.0f, 0.0f}, .spherical_coord{Math::Vector2{Math::radians(60.0f), 0.0f}, 30.0f}
            },
            GameKeyboardController{}
        );
    }

    // Global settings
    Core::EntityId global_settings_id;
    {
        global_settings_id = Core::create_entity(
            Gfx::GlobalColorSettings{.gamma_inverse{1 / 2.2f}}, Gfx::GlobalLightSettings{.light_attenuation{0.2f}}
        );
    }

    // Directional light 1
    {
        Core::create_entity(
            Core::Transform{.position{Math::normalize(Math::Vector3{1.0f, 1.0f, -1.0f})}},
            Gfx::DirectionalLight{},
            Sunlight{
                .light_intensities{sunlight_intensities},
                .day_length{30.0f},
                .time{5.0f},
                .camera_id{camera_id},
                .light_settings_id{global_settings_id}
            }
        );
    }

    // Point light 1
    {
        auto material_id = next_material_id++;
        Gfx::create_material<Gfx::NoLightingColorMaterial>(material_id, NO_LIGHTING_COLOR_SHADER);

        auto id = Core::create_entity(
            Core::Transform{.position{10.0f, 3.0f, 1.0f}, .scale{0.2f}},
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::ModelRef{Gfx::CUBE_MODEL_ID},
            Gfx::Renderer{},
            Gfx::PointLight{.light_intensity{0.6f, 0.6f, 0.6f, 1.0f}},
            Core::PathFollower{
                .path{point_light_path_1},
                .target_position{point_light_path_1[0]},
                .move_speed{6.0f},
                .repeat_path{true}
            }
        );

        Gfx::init_renderer(id);
    }

    // Point light 2
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<Gfx::NoLightingColorMaterial>(material_id, NO_LIGHTING_COLOR_SHADER);
        material.color = {0.0f, 0.0f, 1.0f, 1.0f};

        auto id = Core::create_entity(
            Core::Transform{
                .position{-0.600002f, 6.20000f, 0.300000f},
                .orientation{1.00000f, 0.00000f, 0.00000f, 0.00000f},
                .scale{0.200000f, 0.200000f, 0.200000f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::ModelRef{Gfx::CUBE_MODEL_ID},
            Gfx::Renderer{},
            Gfx::PointLight{.light_intensity{0.0f, 0.0f, 0.7f, 1.0f}},
            Core::PathFollower{
                .path{point_light_path_2},
                .target_position{point_light_path_2[0]},
                .move_speed{7.0f},
                .repeat_path{true}
            }
        );

        Gfx::init_renderer(id);
    }

    // Point light 3
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<Gfx::NoLightingColorMaterial>(material_id, NO_LIGHTING_COLOR_SHADER);
        material.color = {1.0f, 0.0f, 0.0f, 1.0f};

        auto id = Core::create_entity(
            Core::Transform{
                .position{point_light_path_3[0]},
                .orientation{1.00000f, 0.00000f, 0.00000f, 0.00000f},
                .scale{0.200000f, 0.200000f, 0.200000f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::ModelRef{Gfx::CUBE_MODEL_ID},
            Gfx::Renderer{},
            Gfx::PointLight{.light_intensity{0.7f, 0.0f, 0.0f, 1.0f}},
            Core::PathFollower{
                .path{point_light_path_3},
                .target_position{point_light_path_3[1]},
                .move_speed{5.0f},
                .repeat_path{true}
            }
        );

        Gfx::init_renderer(id);
    }

    // Ground
    {
        auto material_id = next_material_id++;
        Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);

        material_buffer_writer[0] = {.specular_color{0.0f}, .surface_shininess{1.0f}};

        auto id = Core::create_entity(
            Core::Transform{
                .orientation{Math::axis_angle_quaternion({1.0f, 0.0f, 0.0f}, Math::radians(-90.0f))}, .scale{0.2f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(0).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::ModelRef{ground_model_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND);
    }

    // Cylinder
    {
        auto model_id = Gfx::CYLINDER_MODEL_ID;
        auto material_id = next_material_id++;
        auto &material =
            Gfx::create_material<FragmentLightingColorMaterial>(material_id, FRAGMENT_LIGHTING_COLOR_SHADER);
        material.diffuse_color = {0.968f, 0.141f, 0.019f, 1.0f};

        material_buffer_writer[1] = {.specular_color{0.22f, 0.20f, 0.0f, 1.0f}, .surface_shininess{0.3f}};

        auto id = Core::create_entity(
            Core::Transform{
                .position{-0.300010f, 7.40000f, -2.30000f},
                .orientation{1.00000f, 0.00000f, 0.00000f, 0.00000f},
                .scale{4.00000f, 4.00000f, 4.00000f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(1).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::ModelRef{model_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND);
    }

    // Cube 1
    {
        auto model_id = Gfx::CUBE_MODEL_ID;
        auto material_id = next_material_id++;
        Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);

        material_buffer_writer[2] = {.specular_color{0.3f, 0.3f, 0.3f, 1.0f}, .surface_shininess{0.3f}};

        auto id = Core::create_entity(
            Core::Transform{
                .position{12.9000f, 3.40000f, 17.6000f},
                .orientation{0.859259f, -0.268912f, -0.229626f, -0.369666f},
                .scale{4.40000f, 4.40000f, 4.40000f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(2).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::ModelRef{model_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND);
    }

    // Cube 2
    {
        auto model_id = Gfx::CUBE_MODEL_ID;
        auto material_id = next_material_id++;
        Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);

        material_buffer_writer[3] = {.specular_color{1.0f}, .surface_shininess{0.01f}};

        auto id = Core::create_entity(
            Core::Transform{
                .position{-16.0000f, 7.90000f, -14.2000f},
                .orientation{0.594317f, -0.573115f, -0.382432f, -0.414812f},
                .scale{4.50000f, 4.40000f, 13.1000f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(3).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::ModelRef{model_id},
            Gfx::Renderer{}
        );

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
    process_components(update_sunlight);
}
} // namespace Game
