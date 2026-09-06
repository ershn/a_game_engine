#include <algorithm>
#include <array>
#include <tuple>
#include <utility>
#include <vector>

#include "default_materials.hpp"
#include "default_meshes.hpp"
#include "default_shaders.hpp"
#include "ecs.hpp"
#include "error_handling.hpp"
#include "framebuffer.hpp"
#include "inplace_list.hpp"
#include "lighting.hpp"
#include "opengl/opengl_api.hpp"
#include "renderbuffer.hpp"
#include "rendering.hpp"
#include "spherical_camera.hpp"
#include "transformations.hpp"
#include "tuple.hpp"

#include "game/game_controllers.hpp"
#include "game/render_to_texture_scene.hpp"

namespace Game
{
using namespace Age;

struct RenderPipelineData
{
    Gfx::RenderbufferId color_renderbuffer_id{};
    Gfx::RenderbufferId depth_renderbuffer_id{};
    Gfx::FramebufferId framebuffer_id{};
    Gfx::TextureId color_texture_id{};
};

template <typename T>
T &get_global()
{
    static T global{};
    return global;
}

void RenderToTextureScene::init()
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};

    auto &render_pipeline_data = get_global<RenderPipelineData>();

    render_pipeline_data.color_renderbuffer_id =
        Gfx::create_renderbuffer({.width = 1280, .height = 720, .format = Gfx::ImageFormat::R8G8B8A8_UNORM});
    render_pipeline_data.depth_renderbuffer_id =
        Gfx::create_renderbuffer({.width = 1280, .height = 720, .format = Gfx::ImageFormat::D16_UNORM});

    render_pipeline_data.framebuffer_id = Gfx::create_framebuffer(
        {.color_attachments = {Gfx::FramebufferAttachment{
             static_cast<Gfx::RenderTargetId>(render_pipeline_data.color_renderbuffer_id)
         }},
         .depth_attachment = {static_cast<Gfx::RenderTargetId>(render_pipeline_data.depth_renderbuffer_id)}}
    );

    render_pipeline_data.color_texture_id =
        Gfx::create_texture_from_framebuffer(render_pipeline_data.framebuffer_id, Gfx::ImageFormat::R8G8B8A8_UNORM);

    auto linear_sampler_id = Gfx::create_sampler(
        Gfx::SamplerParams{.flags{
            .texture_mag_filter{Gfx::TextureMagFilter::LINEAR}, .texture_min_filter{Gfx::TextureMinFilter::LINEAR}
        }}
    );

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
            Gfx::CameraStackOrder{.stack = 0, .order_in_stack = 0},
            Gfx::CameraRenderState{.framebuffer_id = render_pipeline_data.framebuffer_id},
            Gfx::CameraClear{.framebuffer_clear{.clear_colors{Math::Vector4{0.75f, 0.75f, 1.0f, 1.0f}}}},
            Gfx::ProjectionUniformBuffer{projection_buffer, projection_buffer.create_range()},
            Input::MouseInput{.motion_sensitivity = 0.005f, .scroll_sensitivity = 0.2f},
            Gfx::SphericalCamera{
                .min_distance = 0.75f,
                .max_distance = 100.0f,
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
            Core::Transform{.position{Math::normalize(Math::Vector3{1.0f, 1.0f, 1.0f})}},
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

    // Plane
    {
        auto [shader_id, _] = Gfx::create_shader<Gfx::LitDiffuseTextureShader>(
            {{Gfx::OGL::ShaderType::VERTEX, "shaders/lit_diffuse_texture.vert"},
             {Gfx::OGL::ShaderType::FRAGMENT, "shaders/lit_diffuse_texture.frag"}}
        );

        auto [material_id, material] =
            Gfx::create_material<Gfx::LitDiffuseTextureMaterial>(shader_id, Gfx::DrawQueue::max_opaque);
        material.light_buffer_range_id = light_buffer_range_id;
        material.texture_id = render_pipeline_data.color_texture_id;
        material.sampler_id = linear_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{
                .position{-1.0f, 0.0f, -3.0f},
                .orientation{Math::axis_angle_quaternion(Math::up<Math::Vector3>, Math::radians(20.0f))},
                .scale = Math::Vector3{16.0f, 9.0f, 1.0f} / 5.0f
            },
            Gfx::LocalToWorldMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::PLANE_MESH_ID},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::WITH_LW_MATRIX);
    }

    // Cube
    {
        auto [shader_id, _1] = Gfx::create_shader<Gfx::UnlitShader>(
            {{Gfx::OGL::ShaderType::VERTEX, "shaders/unlit.vert"},
             {Gfx::OGL::ShaderType::FRAGMENT, "shaders/unlit.frag"}}
        );

        auto [material_id, _2] = Gfx::create_material<Gfx::UnlitMaterial>(shader_id);

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

void RenderToTextureScene::update()
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(Gfx::update_spherical_camera_via_input);
    process_components(Gfx::calc_spherical_camera_view_matrix);

    if (Gfx::has_system_framebuffer_size_changed())
        process_components(Gfx::update_perspective_camera_matrix);
}

void RenderToTextureScene::render()
{
    using Camera = std::tuple<
        const Gfx::CameraStackOrder *,
        const Gfx::CameraRenderState *,
        const Gfx::CameraClear *,
        const Gfx::WorldToViewMatrix *,
        const Gfx::ProjectionUniformBuffer *>;

    Core::InplaceList<Camera, 8> cameras{};
    Core::fill_with_entity_components(cameras);

    std::sort(cameras.begin(), cameras.end(), [](const auto &camera1, const auto &camera2) {
        return Util::get_ref<Gfx::CameraStackOrder>(camera1) < Util::get_ref<Gfx::CameraStackOrder>(camera2);
    });

    auto &render_pipeline_data = get_global<RenderPipelineData>();

    Gfx::for_each_camera_stack(cameras.cbegin(), cameras.cend(), [&render_pipeline_data](auto &camera_it) {
        auto &camera_render_state = *std::get<const Gfx::CameraRenderState *>(*camera_it);
        auto &camera_clear = *std::get<const Gfx::CameraClear *>(*camera_it);
        auto &wv_matrix = *std::get<const Gfx::WorldToViewMatrix *>(*camera_it);
        auto &projection_buffer = *std::get<const Gfx::ProjectionUniformBuffer *>(*camera_it);

        Gfx::update_lighting(wv_matrix);
        Gfx::setup_viewport(camera_render_state, camera_clear);

        auto &draw_call_keys = Gfx::get_layer_draw_calls(camera_render_state.layer);
        Gfx::sort_draw_calls(draw_call_keys);

        Gfx::set_render_targets(render_pipeline_data.framebuffer_id, 0b1);

        auto dc_key_it = draw_call_keys.cbegin();
        auto dc_key_end = draw_call_keys.cend();
        dc_key_it = Gfx::execute_draw_calls<Util::Less>(
            Gfx::DrawQueue::max_opaque, dc_key_it, dc_key_end, wv_matrix, projection_buffer
        );

        Gfx::copy_framebuffer_to_texture(render_pipeline_data.framebuffer_id, render_pipeline_data.color_texture_id);

        Gfx::execute_draw_calls<Util::LessEqual>(
            Gfx::DrawQueue::max, dc_key_it, dc_key_end, wv_matrix, projection_buffer
        );
    });

    Gfx::blit_framebuffer(
        render_pipeline_data.framebuffer_id,
        Math::Rectangle{{}, {1.0f, 1.0f}},
        Gfx::SYSTEM_FRAMEBUFFER_ID,
        Math::Rectangle{{}, {1.0f, 1.0f}},
        0b1,
        0b1,
        false,
        false,
        Gfx::BlitFilter::NEAREST
    );
}
} // namespace Game
