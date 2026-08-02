#include <bit>
#include <functional>
#include <span>

#include "DDS.hpp"
#include "ECS.hpp"
#include "Framebuffer.hpp"
#include "Input.hpp"
#include "Logging.hpp"
#include "Rendering.hpp"
#include "SphericalCamera.hpp"
#include "Texture.hpp"
#include "Time.hpp"
#include "Transformations.hpp"

#include "game/BigPlaneMesh.hpp"
#include "game/CheckerboardScene.hpp"
#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

struct CheckerboardShader : public Gfx::Shader
{
    Gfx::SamplerUniform sampler{};

    CheckerboardShader(GLuint shader_program)
        : Shader{shader_program}
        , sampler{Gfx::OGL::get_uniform_location(shader_program, "_texture")}
    {
    }
};

struct CheckerboardMaterial : public Gfx::Material
{
    Gfx::TextureId texture_id{Gfx::NULL_TEXTURE_ID};
    Gfx::SamplerId sampler_id{Gfx::NULL_SAMPLER_ID};

    CheckerboardMaterial(Gfx::Shader &shader, Gfx::RenderPipelineState render_state, Gfx::DrawQueue draw_queue)
        : Material{shader, render_state, draw_queue}
    {
    }

    void apply_properties() const override
    {
        auto &shader = static_cast<CheckerboardShader &>(this->shader);
        Gfx::bind_texture_and_sampler(shader.sampler, texture_id, sampler_id);
    }
};

struct CheckerboardSceneController
{
    static constexpr Core::ComponentType TYPE{CHECKERBOARD_SCENE_CONTROLLER};

    Input::PressedKeys pressed_keys{};
    Gfx::MaterialId material_id{};
    Gfx::TextureId checkerboard_texture_id{};
    Gfx::TextureId mipmap_texture_id{};
};

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

void fill_with_mipmap_color(std::span<std::byte> buffer, std::size_t mipmap_level)
{
    for (std::size_t index{}; index < buffer.size(); index += 3)
    {
        buffer[index] = std::byte{MIPMAP_COLORS[mipmap_level * 3]};
        buffer[index + 1] = std::byte{MIPMAP_COLORS[mipmap_level * 3 + 1]};
        buffer[index + 2] = std::byte{MIPMAP_COLORS[mipmap_level * 3 + 2]};
    }
}

void update_camera(Gfx::WorldToViewMatrix &view_matrix)
{
    Math::Vector3 target_pos{std::cos(Time::frame_time()) * 0.25f, std::sin(Time::frame_time()) * 0.25f - 5.0f, 20.0f};
    Math::Vector3 camera_pos{std::cos(Time::frame_time()) * 0.25f, 0.5f, 40.0f};

    view_matrix.matrix = Math::look_at_matrix(target_pos, camera_pos, Math::Vector3::up);
}

void control_scene(CheckerboardSceneController &scene_controller)
{
    auto &material = static_cast<CheckerboardMaterial &>(Gfx::get_material(scene_controller.material_id));
    Gfx::SamplerParams sampler_params{Gfx::get_sampler_params(material.sampler_id)};

    if (Input::is_key_pressed(GLFW_KEY_W, scene_controller.pressed_keys))
    {
        material.texture_id = scene_controller.mipmap_texture_id;
        Core::log_info("Using mipmap texture: id = {}", material.texture_id);
    }
    else if (Input::is_key_pressed(GLFW_KEY_F, scene_controller.pressed_keys))
    {
        material.texture_id = scene_controller.checkerboard_texture_id;
        Core::log_info("Using checkerboard texture: id = {}", material.texture_id);
    }

    if (Input::is_key_pressed(GLFW_KEY_S, scene_controller.pressed_keys))
    {
        sampler_params.flags.texture_min_filter = Gfx::TextureMinFilter::NEAREST_MIPMAP_LINEAR;
        Gfx::set_sampler_params(material.sampler_id, sampler_params);
    }
    else if (Input::is_key_pressed(GLFW_KEY_T, scene_controller.pressed_keys))
    {
        sampler_params.flags.texture_min_filter = Gfx::TextureMinFilter::LINEAR_MIPMAP_LINEAR;
        Gfx::set_sampler_params(material.sampler_id, sampler_params);
    }
}

void CheckerBoardScene::init()
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};

    float max_anisotropy{Gfx::get_texture_filtering_max_max_anisotropy()};
    Core::log_info("max anisotropy: {}", max_anisotropy);

    Gfx::TextureData checkerboard_texture_data{};
    if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/checkerboard.dds", checkerboard_texture_data))
        return;

    Gfx::TextureData mipmap_texture_data{};
    {
        constexpr std::size_t MIPMAP_BASE_LEVEL_SIZE{128};

        std::size_t buffer_size{};
        for (std::size_t mipmap_level_size{MIPMAP_BASE_LEVEL_SIZE}; mipmap_level_size > 0; mipmap_level_size >>= 1)
            buffer_size += mipmap_level_size * mipmap_level_size * 3;

        mipmap_texture_data.bytes = std::make_unique_for_overwrite<std::byte[]>(buffer_size);
        mipmap_texture_data.desc = {
            .width{MIPMAP_BASE_LEVEL_SIZE},
            .height{MIPMAP_BASE_LEVEL_SIZE},
            .depth{1},
            .mipmap_level_count{static_cast<std::uint32_t>(std::bit_width(MIPMAP_BASE_LEVEL_SIZE))},
            .count{1},
            .format{Gfx::TextureFormat::R8G8B8_UNORM},
            .type{Gfx::TextureType::TEXTURE_2D}
        };

        std::size_t buffer_offset{0};
        for (std::size_t mipmap_level{0}; mipmap_level < mipmap_texture_data.desc.mipmap_level_count; ++mipmap_level)
        {
            std::size_t mipmap_level_size{MIPMAP_BASE_LEVEL_SIZE >> mipmap_level};
            std::span<std::byte> buffer{
                &mipmap_texture_data.bytes[buffer_offset], mipmap_level_size * mipmap_level_size * 3
            };
            fill_with_mipmap_color(buffer, mipmap_level);
            buffer_offset += buffer.size();
        }
    }

    auto checkerboard_texture_id = Gfx::create_texture(checkerboard_texture_data);
    auto mipmap_texture_id = Gfx::create_texture(mipmap_texture_data);

    auto mipmap_aniso_sampler_id = Gfx::create_sampler(
        Gfx::SamplerParams{
            .max_anisotropy{max_anisotropy},
            .flags{
                .texture_mag_filter{Gfx::TextureMagFilter::LINEAR},
                .texture_min_filter{Gfx::TextureMinFilter::LINEAR_MIPMAP_LINEAR}
            }
        }
    );

    // Camera
    Core::EntityId camera_id;
    {
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto projection_buffer = Gfx::create_uniform_buffer<Gfx::ProjectionBlock>();

        camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}},
            Gfx::ProjectionUniformBuffer{projection_buffer, projection_buffer.create_range()},
            GameKeyboardController{}
        );
    }

    auto [checkerboard_shader_id, _] = Gfx::create_shader<CheckerboardShader>(
        {{Gfx::OGL::ShaderType::VERTEX, "shaders/game/checkerboard.vert"},
         {Gfx::OGL::ShaderType::FRAGMENT, "shaders/game/checkerboard.frag"}}
    );

    // Plane
    {
        auto mesh_id = next_mesh_id++;
        Gfx::create_mesh<1>(mesh_id, std::function{create_big_plane_mesh});

        auto [material_id, material] = Gfx::create_material<CheckerboardMaterial>(checkerboard_shader_id);
        material.texture_id = checkerboard_texture_id;
        material.sampler_id = mipmap_aniso_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{},
            Gfx::LocalToWorldMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{mesh_id},
            Gfx::Renderer{},
            CheckerboardSceneController{
                .material_id{material_id},
                .checkerboard_texture_id{checkerboard_texture_id},
                .mipmap_texture_id{mipmap_texture_id},
            }
        );

        Gfx::init_renderer(id, Gfx::WITH_LW_MATRIX);
    }
}

void CheckerBoardScene::update()
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(update_camera);
    process_components(control_scene);

    if (Gfx::has_system_framebuffer_size_changed())
        process_components(Gfx::update_perspective_camera_matrix);
}

void CheckerBoardScene::render()
{
    Gfx::render_scene();
}
} // namespace Game
