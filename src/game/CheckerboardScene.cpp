#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "Color.hpp"
#include "DDS.hpp"
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

struct CheckerboardSceneController
{
    static constexpr Core::ComponentType TYPE{CHECKERBOARD_SCENE_CONTROLLER};

    GLuint checkerboard_texture;
    GLuint mipmap_texture;
    GLuint texture_sampler;
};

constexpr int surface_texture_unit{1};

// clang-format off
const GLubyte MIPMAP_COLORS[] = {
    0xFF, 0xFF, 0x00,
    0xFF, 0x00, 0xFF,
    0x00, 0xFF, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00,
    0xFF, 0xFF, 0xFF,
};
// clang-format on

void fill_with_mipmap_color(std::span<GLubyte> buffer, std::size_t mipmap_level)
{
    for (std::size_t index{}; index < buffer.size(); index += 3)
    {
        buffer[index] = MIPMAP_COLORS[mipmap_level * 3];
        buffer[index + 1] = MIPMAP_COLORS[mipmap_level * 3 + 1];
        buffer[index + 2] = MIPMAP_COLORS[mipmap_level * 3 + 2];
    }
}

void update_camera(Gfx::WorldToViewMatrix &view_matrix)
{
    Math::Vector3 target_pos{std::cos(Time::frame_time()) * 0.25f, std::sin(Time::frame_time()) * 0.25f - 5.0f, 20.0f};
    Math::Vector3 camera_pos{std::cos(Time::frame_time()) * 0.25f, 0.5f, 40.0f};

    view_matrix.matrix = Math::look_at_matrix(target_pos, camera_pos, Math::Vector3::up);
}

void control_scene(const CheckerboardSceneController &scene_controller)
{
    if (Input::is_key_down(GLFW_KEY_W))
    {
        glActiveTexture(GL_TEXTURE0 + surface_texture_unit);
        glBindTexture(GL_TEXTURE_2D, scene_controller.mipmap_texture);
    }
    else if (Input::is_key_down(GLFW_KEY_F))
    {
        glActiveTexture(GL_TEXTURE0 + surface_texture_unit);
        glBindTexture(GL_TEXTURE_2D, scene_controller.checkerboard_texture);
    }

    if (Input::is_key_down(GLFW_KEY_S))
    {
        glSamplerParameteri(scene_controller.texture_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    }
    else if (Input::is_key_down(GLFW_KEY_T))
    {
        glSamplerParameteri(scene_controller.texture_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
}

void CheckerBoardScene::init() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{0};
    Gfx::MaterialId next_material_id{0};
    Gfx::UniformBufferId next_uniform_buffer_id{0};

    GLfloat max_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);
    Core::log_info("max anisotropy: {}", max_anisotropy);

    Gfx::TextureData checkerboard_texture_data{};
    if (!Gfx::load_texture_from_dds_file("assets/game/textures/checkerboard.dds", checkerboard_texture_data))
        return;

    GLuint mipmap_texture;
    {
        glGenTextures(1, &mipmap_texture);
        glBindTexture(GL_TEXTURE_2D, mipmap_texture);

        constexpr std::size_t MIPMAP_SIZE{128};
        auto buffer = std::make_unique<GLubyte[]>(MIPMAP_SIZE * MIPMAP_SIZE * 3);

        GLint prev_unpack_alignment;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prev_unpack_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        std::size_t mipmap_level{0};
        for (std::size_t mipmap_size{MIPMAP_SIZE}; mipmap_size > 0; mipmap_size >>= 1, ++mipmap_level)
        {
            fill_with_mipmap_color({buffer.get(), mipmap_size * mipmap_size * 3}, mipmap_level);

            glTexImage2D(
                GL_TEXTURE_2D,
                static_cast<GLint>(mipmap_level),
                GL_RGB8,
                static_cast<GLsizei>(mipmap_size),
                static_cast<GLsizei>(mipmap_size),
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                buffer.get()
            );
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(mipmap_level) - 1);

        glPixelStorei(GL_UNPACK_ALIGNMENT, prev_unpack_alignment);
    }

    GLuint checkerboard_texture;
    {
        glGenTextures(1, &checkerboard_texture);
        glBindTexture(GL_TEXTURE_2D, checkerboard_texture);

        for (const auto &mipmap_level : checkerboard_texture_data)
        {
            glTexImage2D(
                GL_TEXTURE_2D,
                mipmap_level.level,
                GL_RGB8,
                mipmap_level.width,
                mipmap_level.height,
                0,
                GL_BGRA,
                GL_UNSIGNED_INT_8_8_8_8_REV,
                mipmap_level.bytes.data()
            );
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, checkerboard_texture_data.mipmap_level_count - 1);
    }

    glActiveTexture(GL_TEXTURE0 + surface_texture_unit);
    glBindTexture(GL_TEXTURE_2D, checkerboard_texture);

    GLuint texture_sampler;
    glGenSamplers(1, &texture_sampler);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameterf(texture_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_anisotropy);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindSampler(surface_texture_unit, texture_sampler);

    // Camera
    Core::EntityId camera_id;
    {
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto projection_buffer_id = next_uniform_buffer_id++;
        auto &projection_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionBlock>>(projection_buffer_id);

        auto lights_buffer_id = next_uniform_buffer_id++;
        auto &lights_buffer = Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::LightsBlock>>(lights_buffer_id);

        camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}},
            Gfx::ProjectionBufferBlockRef{projection_buffer.get_block()},
            Gfx::LightsBufferBlockRef{lights_buffer.get_block()},
            GameKeyboardController{}
        );
    }

    // Global settings
    Core::EntityId global_settings_id;
    {
        global_settings_id = Core::create_entity(
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
        material.texture_unit = surface_texture_unit;

        auto id = Core::create_entity(
            Core::Transform{},
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{big_plane_mesh_id},
            Gfx::Renderer{},
            CheckerboardSceneController{
                .checkerboard_texture{checkerboard_texture},
                .mipmap_texture{mipmap_texture},
                .texture_sampler{texture_sampler}
            }
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_LV_MATRIX);
    }
}

void CheckerBoardScene::update() const
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(update_camera);
    process_components(control_scene);

    if (Gfx::has_framebuffer_size_changed())
        process_components(Gfx::update_perspective_camera_matrix);
}
} // namespace Game
