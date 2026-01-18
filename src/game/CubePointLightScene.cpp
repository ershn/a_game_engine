#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "Camera.hpp"
#include "DDS.hpp"
#include "DefaultMaterials.hpp"
#include "DefaultMeshes.hpp"
#include "DefaultShaders.hpp"
#include "ECS.hpp"
#include "ErrorHandling.hpp"
#include "Input.hpp"
#include "Lighting.hpp"
#include "OpenGL.hpp"
#include "Path.hpp"
#include "Rendering.hpp"
#include "SphericalCamera.hpp"
#include "Time.hpp"
#include "Transformations.hpp"

#include "game/CubePointLightScene.hpp"
#include "game/DioramaMesh.hpp"
#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

namespace
{
const Math::Vector3 s_space_axes_positions[] = {
    {0.0f, 0.0f, 0.0f},
    {0.5f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.5f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.5f},
};

const Math::Vector3 s_space_axes_colors[] = {
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
};

void create_space_axes_mesh(Gfx::MeshBuffers &mesh_buffers, std::span<Gfx::DrawCommand, 1> draw_commands)
{
    Gfx::create_arrays_mesh(
        s_space_axes_positions,
        s_space_axes_colors,
        nullptr,
        nullptr,
        std::size(s_space_axes_positions),
        Gfx::OGL::RenderingMode::LINES,
        mesh_buffers,
        draw_commands[0]
    );
}

struct CubePointLightBlock
{
    Math::Vector4 view_position{};
    Math::Matrix4 view_to_light_matrix{};
};

struct CubePointLightShader : public Gfx::Shader
{
    Gfx::UniformBlock light_block{};
    Gfx::UniformBlock cube_point_light_block{};
    Gfx::SamplerUniform sampler{};
    Gfx::SamplerUniform cube_point_light_sampler{};

    CubePointLightShader(GLuint shader_program)
        : Gfx::Shader{shader_program, {.lv_normal_matrix = true}}
        , light_block{Gfx::OGL::get_uniform_block_index(shader_program, "LightBlock")}
        , cube_point_light_block{Gfx::OGL::get_uniform_block_index(shader_program, "CubePointLightBlock")}
        , sampler{Gfx::OGL::get_uniform_location(shader_program, "_texture")}
        , cube_point_light_sampler{Gfx::OGL::get_uniform_location(shader_program, "_cubePointLightTexture")}
    {
    }
};

struct CubePointLightMaterial : public Gfx::Material
{
    Gfx::UniformBufferRangeId light_buffer_range_id{};
    Gfx::UniformBufferRangeId cube_point_light_buffer_range_id{};
    Gfx::TextureId texture_id{Gfx::NULL_TEXTURE_ID};
    Gfx::SamplerId sampler_id{Gfx::NULL_SAMPLER_ID};
    Gfx::TextureId cube_map_texture_id{Gfx::NULL_TEXTURE_ID};
    Gfx::SamplerId cube_map_sampler_id{Gfx::NULL_SAMPLER_ID};

    CubePointLightMaterial(Gfx::Shader &shader)
        : Gfx::Material{shader}
    {
    }

    void apply_properties() const override
    {
        auto &shader = static_cast<CubePointLightShader &>(this->shader);
        bind_uniform_buffer_range(shader.shader_program, shader.light_block, light_buffer_range_id);
        bind_uniform_buffer_range(
            shader.shader_program, shader.cube_point_light_block, cube_point_light_buffer_range_id
        );
        bind_texture_and_sampler(shader.sampler, texture_id, sampler_id);
        bind_texture_and_sampler(shader.cube_point_light_sampler, cube_map_texture_id, cube_map_sampler_id);
    }
};

struct RotationOverTime
{
    static constexpr Core::ComponentType TYPE{ROTATION_OVER_TIME};

    Math::Vector3 axis{};
    float angle{};
};

void rotate_over_time(Core::Transform &transform, const RotationOverTime &rotation_over_time)
{
    transform.orientation *=
        Math::axis_angle_quaternion(rotation_over_time.axis, rotation_over_time.angle * Time::delta_time());
}

struct CubePointLight
{
    static constexpr Core::ComponentType TYPE{CUBE_POINT_LIGHT};

    Core::EntityId camera_id{};
    float vertical_fov{};
    Gfx::UniformBuffer<CubePointLightBlock> uniform_buffer;
    Gfx::UniformBufferRangeId uniform_buffer_range_id{};
};

void calc_cube_point_light_matrix(CubePointLight &cube_point_light, const Core::Transform &transform)
{
    const auto &world_to_view_matrix =
        Core::get_entity_component<Gfx::WorldToViewMatrix>(cube_point_light.camera_id).matrix;

    Math::Vector4 light_view_position{world_to_view_matrix * Math::Vector4{transform.position, 1.0f}};
    Math::Matrix4 view_to_light_matrix{Core::transform_matrix(transform).invert() * world_to_view_matrix.inverted()};

    cube_point_light.uniform_buffer.update(
        {.view_position = light_view_position, .view_to_light_matrix = view_to_light_matrix}
    );
}
} // namespace

void CubePointLightScene::init() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{0};
    Gfx::MaterialId next_material_id{0};
    Gfx::TextureId next_texture_id{0};
    Gfx::SamplerId next_sampler_id{0};

    auto space_axes_mesh_id = next_mesh_id++;
    Gfx::create_mesh<1>(space_axes_mesh_id, std::function{create_space_axes_mesh});

    auto unlit_shader_id = next_shader_id++;
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/unlit.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/unlit.frag"}
        };
        Gfx::create_shader<Gfx::UnlitShader>(unlit_shader_id, shader_assets);
    }

    auto unlit_material_id = next_material_id++;
    Gfx::create_material<Gfx::UnlitMaterial>(unlit_material_id, unlit_shader_id);

    auto cube_point_light_shader_id = next_shader_id++;
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/game/cube_point_light.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/game/cube_point_light.frag"}
        };
        Gfx::create_shader<CubePointLightShader>(cube_point_light_shader_id, shader_assets);
    }

    Gfx::TextureId concrete_texture_id{next_texture_id++};
    {
        Gfx::TextureData texture_data{};
        if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/concrete.dds", texture_data))
            return;

        Gfx::load_texture(concrete_texture_id, texture_data, {.force_srgb_internal_format = true});
    }
    Gfx::TextureId dirt_texture_id{next_texture_id++};
    {
        Gfx::TextureData texture_data{};
        if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/dirt.dds", texture_data))
            return;

        Gfx::load_texture(dirt_texture_id, texture_data, {.force_srgb_internal_format = true});
    }
    Gfx::TextureId stone_texture_id{next_texture_id++};
    {
        Gfx::TextureData texture_data{};
        if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/stone.dds", texture_data))
            return;

        Gfx::load_texture(stone_texture_id, texture_data, {.force_srgb_internal_format = true});
    }
    Gfx::TextureId rough_stone_texture_id{next_texture_id++};
    {
        Gfx::TextureData texture_data{};
        if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/rough_stone.dds", texture_data))
            return;

        Gfx::load_texture(rough_stone_texture_id, texture_data, {.force_srgb_internal_format = true});
    }
    Gfx::TextureId wood_texture_id{next_texture_id++};
    {
        Gfx::TextureData texture_data{};
        if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/wood.dds", texture_data))
            return;

        Gfx::load_texture(wood_texture_id, texture_data, {.force_srgb_internal_format = true});
    }
    Gfx::TextureId cube_map_texture_id{next_texture_id++};
    {
        Gfx::TextureData texture_data{};
        if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/planetarium.dds", texture_data))
            return;

        Gfx::load_texture(cube_map_texture_id, texture_data);
    }

    Gfx::SamplerId linear_sampler_id{next_sampler_id++};
    Gfx::create_sampler(
        linear_sampler_id,
        Gfx::SamplerParams{.flags{
            .texture_mag_filter{Gfx::TextureMagFilter::LINEAR}, .texture_min_filter{Gfx::TextureMinFilter::LINEAR}
        }}
    );

    Gfx::SamplerId cube_map_sampler_id{next_sampler_id++};
    Gfx::create_sampler(
        cube_map_sampler_id,
        Gfx::SamplerParams{.flags{
            .texture_wrap_s = Gfx::TextureWrapMode::CLAMP_TO_EDGE,
            .texture_wrap_t = Gfx::TextureWrapMode::CLAMP_TO_EDGE,
            .texture_mag_filter = Gfx::TextureMagFilter::LINEAR,
            .texture_min_filter = Gfx::TextureMinFilter::LINEAR
        }}
    );

    auto light_buffer = Gfx::create_uniform_buffer<Gfx::LightBlock>();
    auto light_buffer_range_id = light_buffer.create_range();

    // Camera
    Core::EntityId camera_id{};
    {
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto projection_buffer = Gfx::create_uniform_buffer<Gfx::ProjectionBlock>();

        camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{.matrix{1.0f}},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{.clear_color{0.8f, 0.8f, 0.8f, 1.0f}},
            Gfx::ProjectionUniformBuffer{projection_buffer, projection_buffer.create_range()},
            Input::MouseInput{.motion_sensitivity{0.005f}},
            Gfx::SphericalCamera{
                .origin{0.0f, 0.0f, 10.0f},
                .spherical_coord{25.0f, Math::Vector2{Math::radians(78.0f), Math::radians(30.0f)}}
            },
            GameKeyboardController{}
        );
    }

    // Light settings
    auto light_settings_id = Core::create_entity(
        Gfx::LightSettings{
            .ambient_light_intensity{Math::Vector4{0.2f, 0.2f, 0.2f, 1.0f}},
            .light_attenuation{1.0f / (30.0f * 30.0f)},
            .max_intensity{2.0f}
        }
    );

    // Directional light
    auto directional_light_id = Core::create_entity(
        Core::Transform{.position{Math::normalize(Math::Vector3{-0.2f, 0.5f, 0.5f})}},
        Gfx::DirectionalLight{.light_intensity{Math::Vector4{0.2f, 0.2f, 0.2f, 1.0f} * 0.0f}}
    );

    // Point light
    auto point_light_id = Core::create_entity(
        Core::Transform{.position{5.0f, 6.0f, 0.5f}},
        Gfx::PointLight{.light_intensity{Math::Vector4{3.5f, 6.5f, 3.0f, 1.0f} * 0.0f}}
    );

    auto cube_point_light_buffer = Gfx::create_uniform_buffer<CubePointLightBlock>();
    auto cube_point_light_buffer_range_id = cube_point_light_buffer.create_range();

    // CubePointLight
    auto cube_point_light_id = Core::create_entity(
        Core::Transform{
            .position{0.0f, 0.0f, 10.0f},
            .orientation{Math::axis_angle_quaternion(Math::Vector3::right, Math::radians(-5.0f))},
            .scale{15.0f}
        },
        Gfx::LocalToViewMatrix{},
        Gfx::MaterialRef{unlit_material_id},
        Gfx::MeshRef{space_axes_mesh_id},
        Gfx::Renderer{},
        CubePointLight{
            .camera_id = camera_id,
            .vertical_fov = 60.0f,
            .uniform_buffer = cube_point_light_buffer,
            .uniform_buffer_range_id = cube_point_light_buffer_range_id
        }
    );

    Gfx::init_renderer(cube_point_light_id, Gfx::WITH_LV_MATRIX);

    // Light group
    Core::create_entity(
        Gfx::LightGroup{
            .light_settings_id = light_settings_id,
            .light_ids = {directional_light_id, point_light_id},
            .light_types = {Core::ComponentType::DIRECTIONAL_LIGHT, Core::ComponentType::POINT_LIGHT},
            .uniform_buffer = light_buffer,
            .uniform_buffer_range_id = light_buffer_range_id,
        }
    );

    // Floor
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<CubePointLightMaterial>(material_id, cube_point_light_shader_id);
        material.light_buffer_range_id = light_buffer_range_id;
        material.cube_point_light_buffer_range_id = cube_point_light_buffer_range_id;
        material.texture_id = dirt_texture_id;
        material.sampler_id = linear_sampler_id;
        material.cube_map_texture_id = cube_map_texture_id;
        material.cube_map_sampler_id = cube_map_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{
                .position{0.0f, -33.5f, 0.0f},
                .orientation{Math::axis_angle_quaternion(Math::Vector3::right, Math::radians(-90.0f))},
                .scale{47.0f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::PLANE_MESH_ID},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::WITH_LV_MATRIX | Gfx::WITH_LV_NORMAL_MATRIX);
    }

    // Diorama
    {
        auto mesh_id = next_mesh_id++;
        Gfx::create_mesh<1>(mesh_id, std::function{create_diorama_mesh});

        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<CubePointLightMaterial>(material_id, cube_point_light_shader_id);
        material.light_buffer_range_id = light_buffer_range_id;
        material.cube_point_light_buffer_range_id = cube_point_light_buffer_range_id;
        material.texture_id = concrete_texture_id;
        material.sampler_id = linear_sampler_id;
        material.cube_map_texture_id = cube_map_texture_id;
        material.cube_map_sampler_id = cube_map_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{.position{0.0f, -10.0f, 0.0f}, .scale{47.0f}},
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{mesh_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::WITH_LV_MATRIX | Gfx::WITH_LV_NORMAL_MATRIX);
    }

    // Leaning bar
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<CubePointLightMaterial>(material_id, cube_point_light_shader_id);
        material.light_buffer_range_id = light_buffer_range_id;
        material.cube_point_light_buffer_range_id = cube_point_light_buffer_range_id;
        material.texture_id = wood_texture_id;
        material.sampler_id = linear_sampler_id;
        material.cube_map_texture_id = cube_map_texture_id;
        material.cube_map_sampler_id = cube_map_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{
                .position{3.0f, -7.0f, -10.0f}, .orientation{0.76604f, 0.64278f, 0.0f, 0.0f}, .scale{5.0f, 5.0f, 45.0f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::WITH_LV_MATRIX | Gfx::WITH_LV_NORMAL_MATRIX);
    }

    // Spinning bar
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<CubePointLightMaterial>(material_id, cube_point_light_shader_id);
        material.light_buffer_range_id = light_buffer_range_id;
        material.cube_point_light_buffer_range_id = cube_point_light_buffer_range_id;
        material.texture_id = rough_stone_texture_id;
        material.sampler_id = linear_sampler_id;
        material.cube_map_texture_id = cube_map_texture_id;
        material.cube_map_sampler_id = cube_map_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{
                .position{-7.0f, 0.0f, 8.0f},
                .orientation{0.791242f, -0.148446f, 0.554035f, 0.212003f},
                .scale{4.0f, 4.0f, 35.0f}
            },
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{},
            RotationOverTime{.axis{Math::Vector3::backward}, .angle{1.0f}}
        );

        Gfx::init_renderer(id, Gfx::WITH_LV_MATRIX | Gfx::WITH_LV_NORMAL_MATRIX);
    }

    // Right bar
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<CubePointLightMaterial>(material_id, cube_point_light_shader_id);
        material.light_buffer_range_id = light_buffer_range_id;
        material.cube_point_light_buffer_range_id = cube_point_light_buffer_range_id;
        material.texture_id = stone_texture_id;
        material.sampler_id = linear_sampler_id;
        material.cube_map_texture_id = cube_map_texture_id;
        material.cube_map_sampler_id = cube_map_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{.position{13.0f, -2.0f, 0.0f}, .scale{4.0f, 4.0f, 10.0f}},
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::WITH_LV_MATRIX | Gfx::WITH_LV_NORMAL_MATRIX);
    }

    // Cube
    {
        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<CubePointLightMaterial>(material_id, cube_point_light_shader_id);
        material.light_buffer_range_id = light_buffer_range_id;
        material.cube_point_light_buffer_range_id = cube_point_light_buffer_range_id;
        material.texture_id = stone_texture_id;
        material.sampler_id = linear_sampler_id;
        material.cube_map_texture_id = cube_map_texture_id;
        material.cube_map_sampler_id = cube_map_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{.position{0.0f, 1.0f, 0.0f}, .scale{3.0f}},
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{},
            RotationOverTime{.axis{Math::Vector3::up}, .angle{1.0f}}
        );

        Gfx::init_renderer(id, Gfx::WITH_LV_MATRIX | Gfx::WITH_LV_NORMAL_MATRIX);
    }
}

void CubePointLightScene::update() const
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(rotate_over_time);
    process_components(Gfx::update_spherical_camera_via_input);
    process_components(Gfx::calc_spherical_camera_view_matrix);
    process_components(calc_cube_point_light_matrix);

    if (Gfx::has_framebuffer_size_changed())
        process_components(Gfx::update_perspective_camera_matrix);
}
} // namespace Game
