#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "Camera.hpp"
#include "Color.hpp"
#include "DDS.hpp"
#include "DefaultMeshes.hpp"
#include "ECS.hpp"
#include "ErrorHandling.hpp"
#include "Input.hpp"
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
        .element_count{std::size(s_plane_indexes)},
    };
}

struct GammaAndTexturesShader : public Gfx::Shader
{
    Gfx::SamplerUniform sampler{};

    GammaAndTexturesShader(GLuint shader_program)
        : Shader{shader_program}
        , sampler{Gfx::OGL::get_uniform_location(shader_program, "_texture")}
    {
    }
};

struct GammaAndTexturesMaterial : public Gfx::Material
{
    Gfx::TextureId texture_id{Gfx::NULL_TEXTURE_ID};
    Gfx::SamplerId sampler_id{Gfx::NULL_SAMPLER_ID};

    GammaAndTexturesMaterial(Gfx::Shader &shader)
        : Material{shader}
    {
    }

    void apply_properties() const override
    {
        auto &shader = static_cast<GammaAndTexturesShader &>(this->shader);
        Gfx::bind_texture_and_sampler(shader.sampler, texture_id, sampler_id);
    }
};

void GammaAndTexturesScene::init() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{0};
    Gfx::MaterialId next_material_id{0};
    Gfx::TextureId next_texture_id{0};
    Gfx::SamplerId next_sampler_id{0};

    Gfx::TextureData gamma_ramp_texture_data{};
    if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/gamma_ramp.dds", gamma_ramp_texture_data))
        return;

    Gfx::TextureId gamma_ramp_texture_id{next_texture_id++};
    Gfx::load_texture(gamma_ramp_texture_id, gamma_ramp_texture_data);

    Gfx::SamplerId nearest_clamp_sampler_id{next_sampler_id++};
    Gfx::create_sampler(
        nearest_clamp_sampler_id,
        Gfx::SamplerParams{.flags{
            .texture_wrap_s = Gfx::TextureWrapMode::CLAMP_TO_EDGE, .texture_wrap_t = Gfx::TextureWrapMode::CLAMP_TO_EDGE
        }}
    );

    // Camera
    Core::EntityId camera_id;
    {
        auto projection_buffer = Gfx::create_uniform_buffer<Gfx::ProjectionBlock>();

        camera_id = Core::create_entity(
            Gfx::WindowSpaceCamera{},
            Gfx::WorldToViewMatrix{.matrix{1.0f}},
            Gfx::ViewToClipMatrix{Gfx::window_space_orthographic_proj_matrix(1, 1)},
            Gfx::CameraRenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}},
            Gfx::ProjectionUniformBuffer{projection_buffer, projection_buffer.create_range()},
            GameKeyboardController{}
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
        material.texture_id = gamma_ramp_texture_id;
        material.sampler_id = nearest_clamp_sampler_id;

        auto id = Core::create_entity(
            Gfx::LocalToViewMatrix{.matrix{1.0f}}, Gfx::MaterialRef{material_id}, Gfx::MeshRef{mesh_id}, Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::WITH_LV_MATRIX);
    }
}

void GammaAndTexturesScene::update() const
{
    using Core::process_components;

    process_components(control_game_via_keyboard);

    if (Gfx::has_framebuffer_size_changed())
        process_components(Gfx::update_window_space_camera_matrix);
}
} // namespace Game
