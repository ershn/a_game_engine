#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "Color.hpp"
#include "DDS.hpp"
#include "ECS.hpp"
#include "ErrorHandling.hpp"
#include "OpenGL.hpp"
#include "Path.hpp"
#include "Rendering.hpp"
#include "SphericalCamera.hpp"
#include "Time.hpp"
#include "Transformations.hpp"
#include "UniformBlocks.hpp"

#include "game/BigPlaneMesh.hpp"
#include "game/CheckerboardScene.hpp"
#include "game/CorridorMesh.hpp"
#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

struct CheckerboardShader : public Gfx::Shader
{
    GLint texture_unit{};

    CheckerboardShader(GLuint shader_program)
        : Shader{shader_program, Gfx::SHADER_LV_MATRIX}
        , texture_unit{Gfx::OGL::get_uniform_location(shader_program, "uTexture")}
    {
    }
};

struct CheckerboardMaterial : public Gfx::Material
{
    int texture_unit{};

    CheckerboardMaterial(const Age::Gfx::Shader &shader)
        : Material{shader}
    {
    }

    void apply_properties() const override
    {
        auto &shader = static_cast<const CheckerboardShader &>(this->shader);
        Gfx::OGL::set_uniform(shader.texture_unit, texture_unit);
    }
};

void CheckerBoardScene::init_entities() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{0};
    Gfx::MaterialId next_material_id{0};
    Gfx::UniformBufferId next_uniform_buffer_id{0};

    constexpr int checkerboard_texture_unit{1};

    Gfx::TextureData checkerboard_texture_data{};
    if (!Gfx::load_texture_from_dds_file("assets/game/textures/checkerboard_texture.dds", checkerboard_texture_data))
        return;

    glActiveTexture(GL_TEXTURE0 + checkerboard_texture_unit);

    GLuint checkerboard_texture;
    glGenTextures(1, &checkerboard_texture);
    glBindTexture(GL_TEXTURE_2D, checkerboard_texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB8,
        checkerboard_texture_data.width,
        checkerboard_texture_data.height,
        0,
        GL_BGRA,
        GL_UNSIGNED_INT_8_8_8_8_REV,
        checkerboard_texture_data.bytes.get()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    GLuint texture_sampler;
    glGenSamplers(1, &texture_sampler);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindSampler(checkerboard_texture_unit, texture_sampler);

    // Camera
    Core::EntityId camera_id;
    {
        Gfx::Camera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto gamma_correction_buffer_id = next_uniform_buffer_id++;
        auto &gamma_correction_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::GammaCorrectionBlock>>(gamma_correction_buffer_id);

        auto projection_buffer_id = next_uniform_buffer_id++;
        auto &projection_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionBlock>>(projection_buffer_id);

        auto lights_buffer_id = next_uniform_buffer_id++;
        auto &lights_buffer = Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::LightsBlock>>(lights_buffer_id);

        camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{},
            Gfx::ViewToClipMatrix{
                Math::perspective_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::RenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}},
            Gfx::GammaCorrectionBufferBlockRef{gamma_correction_buffer.get_block()},
            Gfx::ProjectionBufferBlockRef{projection_buffer.get_block()},
            Gfx::LightsBufferBlockRef{lights_buffer.get_block()},
            GameKeyboardController{}
        );
    }

    // Global settings
    Core::EntityId global_settings_id;
    {
        global_settings_id = Core::create_entity(
            Gfx::GlobalColorSettings{.gamma_inverse{1 / 2.2f}},
            Gfx::GlobalLightSettings{
                .ambient_light_intensity{0.2f, 0.2f, 0.2f, 1.0f},
                .light_attenuation{1.0f / (25.0f * 25.0f)},
                .max_intensity{1.0f}
            }
        );
    }

    auto checkerboard_shader_id = next_shader_id++;
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/game/checkerboard.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/game/checkerboard.frag"}
        };
        Gfx::create_shader<CheckerboardShader>(checkerboard_shader_id, shader_assets);
    }

    // Plane
    {
        auto big_plane_mesh_id = next_mesh_id++;
        Gfx::create_mesh<1>(big_plane_mesh_id, std::function{create_big_plane_mesh});

        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<CheckerboardMaterial>(material_id, checkerboard_shader_id);
        material.texture_unit = checkerboard_texture_unit;

        auto id = Core::create_entity(
            Core::Transform{},
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{big_plane_mesh_id},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_LV_MATRIX);
    }
}

void update_camera(Gfx::WorldToViewMatrix &view_matrix)
{
    Math::Vector3 target_pos{std::cos(Time::frame_time()), std::sin(Time::frame_time()) - 5.0f, 20.0f};
    Math::Vector3 camera_pos{std::cos(Time::frame_time()), 1.0f, 40.0f};

    view_matrix.matrix = Math::look_at_matrix(target_pos, camera_pos, Math::Vector3::up);
}

void CheckerBoardScene::run_systems() const
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(update_camera);
}
} // namespace Game
