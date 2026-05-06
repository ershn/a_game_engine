#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "DefaultMaterials.hpp"
#include "DefaultMeshes.hpp"
#include "DefaultShaders.hpp"
#include "ECS.hpp"
#include "ErrorHandling.hpp"
#include "Framebuffer.hpp"
#include "Lighting.hpp"
#include "OpenGL.hpp"
#include "Rendering.hpp"
#include "SphericalCamera.hpp"
#include "Time.hpp"
#include "Transformations.hpp"

#include "game/ProceduralTextureScene.hpp"
#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

struct TextureShader : public Gfx::Shader
{
    Gfx::UniformBlock light_block{};

    TextureShader(GLuint shader_program)
        : Shader{shader_program, {.lv_normal_matrix = true}}
        , light_block{Gfx::OGL::get_uniform_block_index(shader_program, "LightBlock")}
    {
    }
};

struct TextureMaterial : public Gfx::Material
{
    Gfx::UniformBufferRangeId light_buffer_range_id{};

    TextureMaterial(Age::Gfx::Shader &shader)
        : Material{shader}
    {
    }

    void apply_properties() const override
    {
        auto &shader = static_cast<TextureShader &>(this->shader);
        Gfx::bind_uniform_buffer_range(shader.shader_program, shader.light_block, light_buffer_range_id);
    }
};

void update_sun_position(Core::Transform &transform, const Gfx::DirectionalLight &)
{
    float time{Time::frame_time() * 0.1f};
    transform.position = Math::normalize(Math::Vector3{std::cos(time), 1.0f, -std::sin(time)});
}

void ProceduralTextureScene::init() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{0};
    Gfx::MaterialId next_material_id{0};

    // Camera
    {
        Gfx::PerspectiveCamera camera{
            .near_plane_z{0.1f}, .far_plane_z{100000.0f}, .vertical_fov{Math::radians(50.0f)}
        };

        auto projection_buffer = Gfx::create_uniform_buffer<Gfx::ProjectionBlock>();

        Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}},
            Gfx::ProjectionUniformBuffer{projection_buffer, projection_buffer.create_range()},
            Input::MouseInput{.motion_sensitivity{0.005f}, .scroll_sensitivity{0.2f}},
            Gfx::SphericalCamera{
                .min_distance{0.75f},
                .max_distance{100.0f},
                .origin{0.0f, 0.0f, 0.0f},
                .spherical_coord{2.0f, Math::Vector2{Math::radians(60.0f), 0.0f}}
            },
            GameKeyboardController{}
        );
    }

    auto light_buffer = Gfx::create_uniform_buffer<Gfx::LightBlock>();
    auto light_buffer_range_id = light_buffer.create_range();

    // Light settings
    {
        auto light_settings_id = Core::create_entity(
            Gfx::LightSettings{
                .ambient_light_intensity{0.1f, 0.1f, 0.1f, 1.0f},
                .light_attenuation{1.0f / (25.0f * 25.0f)},
                .max_intensity{1.0f}
            }
        );

        auto directional_light_id = Core::create_entity(
            Core::Transform{.position{Math::normalize(Math::Vector3{1.0f, 1.0f, -1.0f})}},
            Gfx::DirectionalLight{.light_intensity{0.8f, 0.8f, 0.8f, 1.0f}}
        );

        Core::create_entity(
            Gfx::LightGroup{
                .light_settings_id = light_settings_id,
                .light_ids = {directional_light_id},
                .light_types = {Core::ComponentType::DIRECTIONAL_LIGHT},
                .uniform_buffer = light_buffer,
                .uniform_buffer_range_id = light_buffer_range_id,
            }
        );
    }

    // Cube
    {
        auto shader_id = next_shader_id++;
        {
            Gfx::ShaderAsset shader_assets[] = {
                Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/game/star_texture.vert"},
                Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/game/star_texture.frag"}
            };
            Gfx::create_shader<TextureShader>(shader_id, shader_assets);
        }

        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<TextureMaterial>(material_id, shader_id);
        material.light_buffer_range_id = light_buffer_range_id;

        auto id = Core::create_entity(
            Core::Transform{.scale{1.0f, 1.0f, 1.0f}},
            Gfx::LocalToWorldMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::WITH_LW_MATRIX);
    }
}

void ProceduralTextureScene::update() const
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    // process_components(update_sun_position);
    process_components(Gfx::update_spherical_camera_via_input);
    process_components(Gfx::calc_spherical_camera_view_matrix);

    if (Gfx::has_system_framebuffer_size_changed())
        process_components(Gfx::update_perspective_camera_matrix);
}

void ProceduralTextureScene::render() const
{
    Gfx::render_scene();
}
} // namespace Game
