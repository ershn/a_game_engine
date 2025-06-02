#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "Camera.hpp"
#include "Color.hpp"
#include "DDS.hpp"
#include "ECS.hpp"
#include "ErrorHandling.hpp"
#include "Input.hpp"
#include "MeshInstances.hpp"
#include "OpenGL.hpp"
#include "Path.hpp"
#include "Rendering.hpp"
#include "SphericalCamera.hpp"
#include "Time.hpp"
#include "Transformations.hpp"
#include "UniformBlocks.hpp"

#include "game/GammaAndTexturesScene.hpp"
#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

// clang-format off
const GLushort s_plane_vertices[] = {
    400, 200, 0, 65535,
    720, 200, 65535, 65535,
    400, 264, 0, 0,
    720, 264, 65535, 0
};

const GLushort s_plane_indexes[] = {
    0, 1, 2,
    1, 3, 2,
};
// clang-format on

void create_plane_mesh(Gfx::MeshBuffers &mesh_buffers, std::span<Gfx::DrawCommand, 1> draw_commands)
{
    glGenVertexArrays(1, &mesh_buffers.vertex_array_object);
    glBindVertexArray(mesh_buffers.vertex_array_object);

    glGenBuffers(1, &mesh_buffers.vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffers.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_plane_vertices), s_plane_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_UNSIGNED_SHORT, false, sizeof(GLushort) * 4, 0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 2, GL_UNSIGNED_SHORT, true, sizeof(GLushort) * 4, reinterpret_cast<GLvoid *>(sizeof(GLushort) * 2)
    );

    glGenBuffers(1, &mesh_buffers.index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffers.index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_plane_indexes), s_plane_indexes, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    draw_commands[0] = {
        .type{Gfx::DrawCommandType::DRAW_ELEMENTS},
        .rendering_mode{Gfx::OGL::RenderingMode::TRIANGLES},
        .element_count{sizeof(s_plane_indexes) / sizeof(*s_plane_indexes)},
    };
}

struct GammaAndTexturesShader : public Gfx::Shader
{
    GLint texture_unit{};

    GammaAndTexturesShader(GLuint shader_program)
        : Shader{shader_program, Gfx::SHADER_LV_MATRIX}
        , texture_unit{Gfx::OGL::get_uniform_location(shader_program, "uTexture")}
    {
    }
};

struct GammaAndTexturesMaterial : public Gfx::Material
{
    int texture_unit{};

    GammaAndTexturesMaterial(const Age::Gfx::Shader &shader)
        : Material{shader}
    {
    }

    void apply_properties() const override
    {
        auto &shader = static_cast<const GammaAndTexturesShader &>(this->shader);
        Gfx::OGL::set_uniform(shader.texture_unit, texture_unit);
    }
};

constexpr int surface_texture_unit{1};

void GammaAndTexturesScene::init_entities() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{0};
    Gfx::MaterialId next_material_id{0};
    Gfx::UniformBufferId next_uniform_buffer_id{0};

    Gfx::TextureData gamma_ramp_texture_data{};
    if (!Gfx::load_texture_from_dds_file("assets/game/textures/gamma_ramp.dds", gamma_ramp_texture_data))
        return;

    GLuint gamma_ramp_texture;
    glGenTextures(1, &gamma_ramp_texture);
    glBindTexture(GL_TEXTURE_2D, gamma_ramp_texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_SRGB8,
        gamma_ramp_texture_data.width,
        gamma_ramp_texture_data.height,
        0,
        GL_BGRA,
        GL_UNSIGNED_INT_8_8_8_8_REV,
        gamma_ramp_texture_data.bytes.get()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glActiveTexture(GL_TEXTURE0 + surface_texture_unit);
    glBindTexture(GL_TEXTURE_2D, gamma_ramp_texture);

    GLuint texture_sampler;
    glGenSamplers(1, &texture_sampler);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindSampler(surface_texture_unit, texture_sampler);

    // Camera
    Core::EntityId camera_id;
    {
        auto gamma_correction_buffer_id = next_uniform_buffer_id++;
        auto &gamma_correction_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::GammaCorrectionBlock>>(gamma_correction_buffer_id);

        auto projection_buffer_id = next_uniform_buffer_id++;
        auto &projection_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionBlock>>(projection_buffer_id);

        auto lights_buffer_id = next_uniform_buffer_id++;
        auto &lights_buffer = Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::LightsBlock>>(lights_buffer_id);

        camera_id = Core::create_entity(
            Gfx::WindowSpaceCamera{},
            Gfx::WorldToViewMatrix{.matrix{1.0f}},
            Gfx::ViewToClipMatrix{Gfx::window_space_orthographic_proj_matrix(1, 1)},
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

    auto gamma_and_textures_shader_id = next_shader_id++;
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/game/gamma_and_textures.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/game/gamma_and_textures.frag"}
        };
        Gfx::create_shader<GammaAndTexturesShader>(gamma_and_textures_shader_id, shader_assets);
    }

    // Plane
    {
        auto mesh_id = next_mesh_id++;
        Gfx::create_mesh(mesh_id, std::function{create_plane_mesh});

        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<GammaAndTexturesMaterial>(material_id, gamma_and_textures_shader_id);
        material.texture_unit = surface_texture_unit;

        auto id = Core::create_entity(
            Gfx::LocalToViewMatrix{.matrix{1.0f}}, Gfx::MaterialRef{material_id}, Gfx::MeshRef{mesh_id}, Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_LV_MATRIX);
    }
}

void GammaAndTexturesScene::run_systems() const
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
}
} // namespace Game
