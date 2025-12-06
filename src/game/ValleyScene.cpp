#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "Color.hpp"
#include "DDS.hpp"
#include "DefaultMaterials.hpp"
#include "DefaultMeshes.hpp"
#include "DefaultShaders.hpp"
#include "ECS.hpp"
#include "ErrorHandling.hpp"
#include "OpenGL.hpp"
#include "Path.hpp"
#include "Rendering.hpp"
#include "SphericalCamera.hpp"
#include "Transformations.hpp"
#include "UniformBlocks.hpp"

#include "game/GroundMesh.hpp"
#include "game/Meshes.hpp"
#include "game/Processing.hpp"
#include "game/ShadersAndMaterials.hpp"
#include "game/ValleyScene.hpp"

namespace Game
{
using namespace Age;

constexpr Gfx::ShaderId UNLIT_SHADER{0};
constexpr Gfx::ShaderId UNLIT_COLOR_SHADER{1};
constexpr Gfx::ShaderId FRAGMENT_LIGHTING_SHADER{2};
constexpr Gfx::ShaderId FRAGMENT_LIGHTING_COLOR_SHADER{3};

constexpr Gfx::MaterialId UNLIT_MATERIAL{0};

void ValleyScene::init() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{100};
    Gfx::MaterialId next_material_id{100};
    Gfx::UniformBufferId next_uniform_buffer_id{0};

    auto ground_mesh_id = next_mesh_id++;
    Gfx::create_mesh<1>(ground_mesh_id, std::function{create_ground_mesh});

    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/unlit.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/unlit.frag"}
        };
        Gfx::create_shader<Gfx::UnlitShader>(UNLIT_SHADER, shader_assets);
    }
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/unlit_color.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/unlit.frag"}
        };
        Gfx::create_shader<Gfx::UnlitColorShader>(UNLIT_COLOR_SHADER, shader_assets);
    }
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/fragment_lighting.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/fragment_lighting.frag"}
        };
        Gfx::create_shader<FragmentLightingShader>(FRAGMENT_LIGHTING_SHADER, shader_assets);
    }
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/fragment_lighting_color.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/fragment_lighting.frag"}
        };
        Gfx::create_shader<FragmentLightingColorShader>(FRAGMENT_LIGHTING_COLOR_SHADER, shader_assets);
    }

    auto &unlit_material = Gfx::create_material<Gfx::UnlitMaterial>(UNLIT_MATERIAL, UNLIT_SHADER);

    auto projection_buffer_id = next_uniform_buffer_id++;
    auto &projection_buffer =
        Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionBlock>>(projection_buffer_id);

    auto lights_buffer_id = next_uniform_buffer_id++;
    auto &lights_buffer = Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::LightsBlock>>(lights_buffer_id);

    auto material_buffer_id = next_uniform_buffer_id++;
    auto &material_buffer =
        Gfx::create_uniform_buffer<Gfx::ArrayUniformBuffer<Gfx::MaterialBlock>>(material_buffer_id, 4);
    auto material_buffer_writer = Gfx::ArrayUniformBufferWriter{material_buffer};

    constexpr std::size_t TEXTURE_SIZE{512};

    float shininess{0.1f};
    GLubyte gaussian_terms[TEXTURE_SIZE];
    for (std::size_t index{}; index < TEXTURE_SIZE; ++index)
    {
        float cos{index / static_cast<float>(TEXTURE_SIZE - 1ULL)};

        float half_angle{std::acos(cos)};
        float gaussian_exponent{half_angle / shininess};
        gaussian_exponent = -(gaussian_exponent * gaussian_exponent);
        float gaussian_term{std::exp(gaussian_exponent)};

        gaussian_terms[index] = static_cast<GLubyte>(gaussian_term * 255.0f);
    }

    GLuint gaussian_texture;
    glGenTextures(1, &gaussian_texture);
    glBindTexture(GL_TEXTURE_1D, gaussian_texture);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R8, TEXTURE_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, gaussian_terms);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_1D, 0);

    int gaussian_texture_image_unit{1};

    glActiveTexture(GL_TEXTURE0 + gaussian_texture_image_unit);
    glBindTexture(GL_TEXTURE_1D, gaussian_texture);

    GLuint gaussian_texture_sampler;
    glGenSamplers(1, &gaussian_texture_sampler);
    glSamplerParameteri(gaussian_texture_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(gaussian_texture_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(gaussian_texture_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glBindSampler(gaussian_texture_image_unit, gaussian_texture_sampler);

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
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{.clear_color{0.294f, 0.22f, 0.192f, 1.0f}},
            Gfx::ProjectionBufferBlockRef{projection_buffer.get_block()},
            Gfx::LightsBufferBlockRef{lights_buffer.get_block()},
            Input::MouseInput{.motion_sensitivity{0.005f}},
            Gfx::SphericalCamera{
                .origin{0.0f, 2.0f, 0.0f}, .spherical_coord{30.0f, Math::Vector2{Math::radians(60.0f), 0.0f}}
            },
            GameKeyboardController{}
        );
    }

    // Global settings
    Core::EntityId global_settings_id;
    {
        global_settings_id = Core::create_entity(Gfx::GlobalLightSettings{.light_attenuation{0.2f}});
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
        Gfx::create_material<Gfx::UnlitColorMaterial>(material_id, UNLIT_COLOR_SHADER);

        auto id = Core::create_entity(
            Core::Transform{.position{10.0f, 3.0f, 1.0f}, .scale{0.2f}},
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{},
            Gfx::PointLight{.light_intensity{0.6f, 0.6f, 0.6f, 1.0f}},
            Core::PathFollower{
                .path{point_light_path_1},
                .target_position{point_light_path_1[0]},
                .move_speed{6.0f},
                .repeat_path{true}
            }
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_LV_MATRIX);
    }

    // Point light 2
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<Gfx::UnlitColorMaterial>(material_id, UNLIT_COLOR_SHADER);
        material.color = {0.0f, 0.0f, 1.0f, 1.0f};

        auto id = Core::create_entity(
            Core::Transform{
                .position{-0.600002f, 6.20000f, 0.300000f},
                .orientation{1.00000f, 0.00000f, 0.00000f, 0.00000f},
                .scale{0.200000f, 0.200000f, 0.200000f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{},
            Gfx::PointLight{.light_intensity{0.0f, 0.0f, 0.7f, 1.0f}},
            Core::PathFollower{
                .path{point_light_path_2},
                .target_position{point_light_path_2[0]},
                .move_speed{7.0f},
                .repeat_path{true}
            }
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_LV_MATRIX);
    }

    // Point light 3
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<Gfx::UnlitColorMaterial>(material_id, UNLIT_COLOR_SHADER);
        material.color = {1.0f, 0.0f, 0.0f, 1.0f};

        auto id = Core::create_entity(
            Core::Transform{
                .position{point_light_path_3[0]},
                .orientation{1.00000f, 0.00000f, 0.00000f, 0.00000f},
                .scale{0.200000f, 0.200000f, 0.200000f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{},
            Gfx::PointLight{.light_intensity{0.7f, 0.0f, 0.0f, 1.0f}},
            Core::PathFollower{
                .path{point_light_path_3},
                .target_position{point_light_path_3[1]},
                .move_speed{5.0f},
                .repeat_path{true}
            }
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_LV_MATRIX);
    }

    // Ground
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);
        material.gaussian_texture = gaussian_texture_image_unit;

        material_buffer_writer[0] = {.specular_color{0.0f}, .surface_shininess{0.1f}};

        auto id = Core::create_entity(
            Core::Transform{
                .orientation{Math::axis_angle_quaternion({1.0f, 0.0f, 0.0f}, Math::radians(-90.0f))}, .scale{0.2f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block(0).get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::MeshRef{ground_mesh_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(
            id, Gfx::RENDER_WITH_LV_MATRIX | Gfx::RENDER_WITH_LV_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND
        );
    }

    // Cylinder
    {
        auto mesh_id = Gfx::CYLINDER_MESH_ID;
        auto material_id = next_material_id++;
        auto &material =
            Gfx::create_material<FragmentLightingColorMaterial>(material_id, FRAGMENT_LIGHTING_COLOR_SHADER);
        material.gaussian_texture = gaussian_texture_image_unit;
        material.diffuse_color = {0.968f, 0.141f, 0.019f, 1.0f};

        material_buffer_writer[1] = {.specular_color{0.22f, 0.20f, 0.0f, 1.0f}, .surface_shininess{0.1f}};

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
            Gfx::MeshRef{mesh_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(
            id, Gfx::RENDER_WITH_LV_MATRIX | Gfx::RENDER_WITH_LV_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND
        );
    }

    // Cube 1
    {
        auto mesh_id = Gfx::CUBE_MESH_ID;
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);
        material.gaussian_texture = gaussian_texture_image_unit;

        material_buffer_writer[2] = {.specular_color{0.3f, 0.3f, 0.3f, 1.0f}, .surface_shininess{0.1f}};

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
            Gfx::MeshRef{mesh_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(
            id, Gfx::RENDER_WITH_LV_MATRIX | Gfx::RENDER_WITH_LV_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND
        );
    }

    // Cube 2
    {
        auto mesh_id = Gfx::CUBE_MESH_ID;
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<FragmentLightingMaterial>(material_id, FRAGMENT_LIGHTING_SHADER);
        material.gaussian_texture = gaussian_texture_image_unit;

        material_buffer_writer[3] = {.specular_color{1.0f}, .surface_shininess{0.1f}};

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
            Gfx::MeshRef{mesh_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(
            id, Gfx::RENDER_WITH_LV_MATRIX | Gfx::RENDER_WITH_LV_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND
        );
    }

    material_buffer_writer.apply();

    // Impostor spheres
    {
        auto mesh_id = next_mesh_id++;
        Gfx::create_mesh<1>(mesh_id, std::function{create_sphere_impostors_mesh}, 2ULL);

        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/game/sphere_impostor.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::GEOMETRY, "shaders/game/sphere_impostor.geom"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/game/sphere_impostor.frag"}
        };
        auto shader_id = next_shader_id++;
        Gfx::create_shader<SphereImpostorShader>(shader_id, shader_assets);

        auto material_id = next_material_id++;
        Gfx::create_material<SphereImpostorMaterial>(material_id, shader_id);

        auto materials_buffer_id = next_uniform_buffer_id++;
        auto &materials_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Age::Gfx::MaterialsBlock<4>>>(materials_buffer_id);

        auto block = materials_buffer.get_block();
        block = {
            .materials = {
                {.diffuse_color{0.223f, 0.635f, 0.443f, 1.0f}, .specular_color{0.0f}},
                {.diffuse_color{0.968f, 0.141f, 0.019f, 1.0f}, .specular_color{0.0f}}
            }
        };
        Gfx::bind_uniform_buffer(MATERIALS_BLOCK_BINDING, block.get_buffer_range());

        auto id = Core::create_entity(
            Gfx::MeshRef{mesh_id},
            Gfx::MaterialRef{material_id},
            Gfx::Renderer{},
            SphereImpostors{
                .instances =
                    {{.worldPosition{10.80000f, 6.10000f, 0.300018f}, .radius{2.0f}},
                     {.worldPosition{-8.80000f, 3.10000f, 3.300018f}, .radius{1.5f}}},
                .instance_count{2},
                .camera_id{camera_id}
            }
        );

        Gfx::init_renderer(id);
    }
}

void ValleyScene::update() const
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(control_material_via_keyboard);
    process_components(control_transform_via_keyboard);
    process_components(Gfx::update_spherical_camera_via_input);
    process_components(Gfx::calc_spherical_camera_view_matrix);
    process_components(Core::move_along_path);
    process_components(update_sunlight);
    process_components(update_sphere_impostors);

    if (Gfx::has_framebuffer_size_changed())
        process_components(Gfx::update_perspective_camera_matrix);
}
} // namespace Game
