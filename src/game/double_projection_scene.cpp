#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "camera.hpp"
#include "color.hpp"
#include "dds.hpp"
#include "default_materials.hpp"
#include "default_meshes.hpp"
#include "default_shaders.hpp"
#include "ecs.hpp"
#include "error_handling.hpp"
#include "input.hpp"
#include "lighting.hpp"
#include "path.hpp"
#include "rendering.hpp"
#include "spherical_camera.hpp"
#include "time.hpp"
#include "transformations.hpp"
#include "uniform_blocks.hpp"
#include "opengl/opengl_api.hpp"

#include "game/double_projection_scene.hpp"
#include "game/game_controllers.hpp"

namespace Game
{
using namespace Age;

namespace
{
struct SphericalCameraMouseController
{
    static constexpr Core::ComponentType TYPE{SPHERICAL_CAMERA_MOUSE_CONTROLLER};

    int motion_activation_button{};
    bool is_distance_updated{};
};

void update_spherical_camera_via_input(
    const SphericalCameraMouseController &controller,
    const Input::MouseInput &mouse_input,
    Gfx::SphericalCamera &spherical_camera
)
{
    if (Input::is_mouse_button_down(controller.motion_activation_button))
        Gfx::update_spherical_camera_via_input(mouse_input, spherical_camera);
}

struct SphericalCameraViewMatrixUpdater
{
    static constexpr Core::ComponentType TYPE{SPHERICAL_CAMERA_VIEW_MATRIX_UPDATER};
};

void update_spherical_camera_view_matrix(
    const SphericalCameraViewMatrixUpdater &,
    const Gfx::SphericalCamera &spherical_camera,
    Gfx::WorldToViewMatrix &view_matrix
)
{
    Gfx::calc_spherical_camera_view_matrix(spherical_camera, view_matrix);
}

struct ViewMatrixReader
{
    static constexpr Core::ComponentType TYPE{VIEW_MATRIX_READER};

    Core::EntityId entity_id{};
};

void read_view_matrix(const ViewMatrixReader &reader, Gfx::WorldToViewMatrix &view_matrix)
{
    view_matrix = Core::get_entity_component<const Gfx::WorldToViewMatrix>(reader.entity_id);
}

struct PostProjectionRotation
{
    static constexpr Core::ComponentType TYPE{POST_PROJECTION_ROTATION};
};

void rotate_in_post_proj_space(
    const PostProjectionRotation &,
    const Gfx::SphericalCamera &spherical_camera,
    Gfx::ViewToClipMatrix &view_to_clip_matrix,
    const Gfx::ProjectionUniformBuffer &projection_uniform_buffer
)
{
    const Math::Vector2 &rotation_angles{spherical_camera.spherical_coord.angles};
    view_to_clip_matrix.matrix =
        // Clip space is left-handed so we need to transpose the matrix to keep the same rotation directions
        Math::affine_yx_rotation_matrix(rotation_angles.y, rotation_angles.x - Math::PI * 0.5f).transpose() *
        view_to_clip_matrix.matrix;

    projection_uniform_buffer.buffer.update({view_to_clip_matrix.matrix});
}
} // namespace

void DoubleProjectionScene::init()
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};

    auto [unlit_shader_id, _1] = Gfx::create_shader<Gfx::UnlitShader>(
        {{Gfx::OGL::ShaderType::VERTEX, "shaders/unlit.vert"}, {Gfx::OGL::ShaderType::FRAGMENT, "shaders/unlit.frag"}}
    );

    auto [lit_diffuse_texture_shader_id, _2] = Gfx::create_shader<Gfx::LitDiffuseTextureShader>(
        {{Gfx::OGL::ShaderType::VERTEX, "shaders/lit_diffuse_texture.vert"},
         {Gfx::OGL::ShaderType::FRAGMENT, "shaders/lit_diffuse_texture.frag"}}
    );

    Gfx::TextureData checkerboard_texture_data{};
    if (!Gfx::read_texture_data_from_dds_file("assets/game/textures/checkerboard.dds", checkerboard_texture_data))
        return;

    auto checkerboard_texture_id = Gfx::create_texture(checkerboard_texture_data);

    auto linear_sampler_id = Gfx::create_sampler(
        Gfx::SamplerParams{.flags{
            .texture_mag_filter{Gfx::TextureMagFilter::LINEAR}, .texture_min_filter{Gfx::TextureMinFilter::LINEAR}
        }}
    );

    Gfx::ViewportId left_viewport_id{Gfx::create_viewport({.position{0.0f, 0.0f}, .size{0.5f, 1.0f}})};
    Gfx::ViewportId right_viewport_id{Gfx::create_viewport({.position{0.5f, 0.0f}, .size{0.5f, 1.0f}})};

    // Left camera
    Core::EntityId left_camera_id;
    {
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto projection_buffer = Gfx::create_uniform_buffer<Gfx::ProjectionBlock>();

        left_camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{.matrix{1.0f}},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}, .viewport_id{left_viewport_id}},
            Gfx::ProjectionUniformBuffer{projection_buffer, projection_buffer.create_range()},
            Input::MouseInput{.motion_sensitivity{0.005f}},
            SphericalCameraMouseController{.motion_activation_button{GLFW_MOUSE_BUTTON_LEFT}},
            Gfx::SphericalCamera{
                .origin{0.0f, 0.0f, 0.0f},
                .spherical_coord{5.0f, Math::Vector2{Math::radians(60.0f), Math::radians(20.0f)}}
            },
            SphericalCameraViewMatrixUpdater{},
            GameKeyboardController{}
        );
    }

    // Right camera
    {
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto projection_buffer = Gfx::create_uniform_buffer<Gfx::ProjectionBlock>();

        Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{.matrix{1.0f}},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}, .viewport_id{right_viewport_id}},
            Gfx::ProjectionUniformBuffer{projection_buffer, projection_buffer.create_range()},
            Input::MouseInput{.motion_sensitivity{0.005f}},
            SphericalCameraMouseController{.motion_activation_button{GLFW_MOUSE_BUTTON_RIGHT}},
            Gfx::SphericalCamera{
                .origin{0.0f, 0.0f, 0.0f},
                .spherical_coord{15.0f, Math::Vector2{Math::radians(90.0f), Math::radians(40.0f)}}
            },
            ViewMatrixReader{.entity_id{left_camera_id}},
            PostProjectionRotation{}
        );
    }

    auto light_buffer = Gfx::create_uniform_buffer<Gfx::LightBlock>();
    auto light_buffer_range_id = light_buffer.create_range();

    // Light settings
    {
        auto light_settings_id = Core::create_entity(
            Gfx::LightSettings{
                .ambient_light_intensity{0.2f, 0.2f, 0.2f, 1.0f},
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
        auto [material_id, material] = Gfx::create_material<Gfx::LitDiffuseTextureMaterial>(
            lit_diffuse_texture_shader_id,
            Gfx::RenderPipelineStateMask{.depth_clamping = true},
            Gfx::RenderPipelineState{.depth_clamping = true}
        );
        material.light_buffer_range_id = light_buffer_range_id;
        material.texture_id = checkerboard_texture_id;
        material.sampler_id = linear_sampler_id;

        auto id = Core::create_entity(
            Core::Transform{},
            Gfx::LocalToWorldMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::WITH_LW_MATRIX);
    }

    Core::process_components(Gfx::calc_spherical_camera_view_matrix);
}

void DoubleProjectionScene::update()
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(update_spherical_camera_via_input);
    process_components(update_spherical_camera_view_matrix);
    process_components(read_view_matrix);
    process_components(Gfx::update_perspective_camera_matrix);
    process_components(rotate_in_post_proj_space);
}

void DoubleProjectionScene::render()
{
    Gfx::render_scene();
}
} // namespace Game
