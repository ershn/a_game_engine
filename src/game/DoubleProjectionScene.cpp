#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "Camera.hpp"
#include "Color.hpp"
#include "DDS.hpp"
#include "DefaultMaterials.hpp"
#include "DefaultMeshes.hpp"
#include "DefaultShaders.hpp"
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

#include "game/DoubleProjectionScene.hpp"
#include "game/Processing.hpp"

namespace Game
{
using namespace Age;

namespace
{
constexpr Gfx::ViewportId LEFT_VIEWPORT_ID{Gfx::USER_VIEWPORT_START_ID};
constexpr Gfx::ViewportId RIGHT_VIEWPORT_ID{Gfx::USER_VIEWPORT_START_ID + 1};

void update_viewports()
{
    unsigned int width, height;
    Gfx::get_framebuffer_size(width, height);

    Gfx::Viewport &left_viewport{Gfx::get_viewport(LEFT_VIEWPORT_ID)};
    left_viewport.width = width / 2;
    left_viewport.height = height;

    Gfx::Viewport &right_viewport{Gfx::get_viewport(RIGHT_VIEWPORT_ID)};
    right_viewport.origin_x = width / 2;
    right_viewport.width = width / 2;
    right_viewport.height = height;
}

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

    Core::EntityId entity_id{Core::NULL_ENTITY_ID};
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
    const Gfx::ProjectionBufferBlockRef &projection_buffer_block
)
{
    const Math::Vector2 &rotation_angles{spherical_camera.spherical_coord.angles};
    view_to_clip_matrix.matrix =
        Math::affine_yx_rotation_matrix(rotation_angles.y, rotation_angles.x - Math::PI * 0.5f).transpose() *
        view_to_clip_matrix.matrix;

    projection_buffer_block = {.view_to_clip_matrix{view_to_clip_matrix.matrix}};
}
} // namespace

void DoubleProjectionScene::init() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{0};
    Gfx::MaterialId next_material_id{0};
    Gfx::UniformBufferId next_uniform_buffer_id{0};

    auto unlit_shader_id = next_shader_id++;
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/unlit.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/unlit.frag"}
        };
        Gfx::create_shader<Gfx::UnlitShader>(unlit_shader_id, shader_assets);
    }

    auto lit_diffuse_texture_shader_id = next_shader_id++;
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/lit_diffuse_texture.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/lit_diffuse_texture.frag"}
        };
        Gfx::create_shader<Gfx::LitDiffuseTextureShader>(lit_diffuse_texture_shader_id, shader_assets);
    }

    Gfx::TextureData checkerboard_texture_data{};
    if (!Gfx::load_texture_from_dds_file("assets/game/textures/checkerboard.dds", checkerboard_texture_data))
        return;

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

    constexpr int checkerboard_texture_unit{1};

    glActiveTexture(GL_TEXTURE0 + checkerboard_texture_unit);
    glBindTexture(GL_TEXTURE_2D, checkerboard_texture);

    GLuint texture_sampler;
    glGenSamplers(1, &texture_sampler);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindSampler(checkerboard_texture_unit, texture_sampler);

    Gfx::create_viewport(LEFT_VIEWPORT_ID);
    Gfx::create_viewport(RIGHT_VIEWPORT_ID);

    auto lights_buffer_id = next_uniform_buffer_id++;
    auto &lights_buffer = Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::LightsBlock>>(lights_buffer_id);

    // Left camera
    Core::EntityId left_camera_id;
    {
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto projection_buffer_id = next_uniform_buffer_id++;
        auto &projection_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionBlock>>(projection_buffer_id);

        left_camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{.matrix{1.0f}},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}, .viewport_id{LEFT_VIEWPORT_ID}},
            Gfx::ProjectionBufferBlockRef{projection_buffer.get_block()},
            Gfx::LightsBufferBlockRef{lights_buffer.get_block()},
            Input::MouseInput{.motion_sensitivity{0.005f}},
            SphericalCameraMouseController{.motion_activation_button{GLFW_MOUSE_BUTTON_LEFT}},
            Gfx::SphericalCamera{
                .origin{0.0f, 0.0f, 0.0f},
                .spherical_coord{Math::Vector2{Math::radians(60.0f), Math::radians(20.0f)}, 5.0f}
            },
            SphericalCameraViewMatrixUpdater{},
            GameKeyboardController{}
        );
    }

    // Right camera
    {
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto projection_buffer_id = next_uniform_buffer_id++;
        auto &projection_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionBlock>>(projection_buffer_id);

        Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{.matrix{1.0f}},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::CameraRenderState{
                .flags{Gfx::DEFAULT_CAMERA_FLAGS | Gfx::DEPTH_CLAMPING},
                .clear_color{0.75f, 0.75f, 1.0f, 1.0f},
                .viewport_id{RIGHT_VIEWPORT_ID}
            },
            Gfx::ProjectionBufferBlockRef{projection_buffer.get_block()},
            Gfx::LightsBufferBlockRef{lights_buffer.get_block()},
            Input::MouseInput{.motion_sensitivity{0.005f}},
            SphericalCameraMouseController{.motion_activation_button{GLFW_MOUSE_BUTTON_RIGHT}},
            Gfx::SphericalCamera{
                .origin{0.0f, 0.0f, 0.0f},
                .spherical_coord{Math::Vector2{Math::radians(60.0f), Math::radians(40.0f)}, 15.0f}
            },
            ViewMatrixReader{.entity_id{left_camera_id}},
            PostProjectionRotation{},
            GameKeyboardController{}
        );
    }

    // Global settings
    {
        Core::create_entity(
            Gfx::GlobalLightSettings{
                .ambient_light_intensity{0.2f, 0.2f, 0.2f, 1.0f},
                .light_attenuation{1.0f / (25.0f * 25.0f)},
                .max_intensity{1.0f}
            }
        );
    }

    // Directional light
    {
        Core::create_entity(
            Core::Transform{.position{Math::normalize(Math::Vector3{1.0f, 1.0f, -1.0f})}},
            Gfx::DirectionalLight{.light_intensity{0.8f, 0.8f, 0.8f, 1.0f}}
        );
    }

    // Cube
    {
        auto material_id = next_material_id++;
        auto &material =
            Gfx::create_material<Gfx::LitDiffuseTextureMaterial>(material_id, lit_diffuse_texture_shader_id);
        material.texture_unit = checkerboard_texture_unit;

        auto id = Core::create_entity(
            Core::Transform{},
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{}
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_LV_MATRIX | Gfx::RENDER_WITH_LV_NORMAL_MATRIX);
    }

    Core::process_components(Gfx::calc_spherical_camera_view_matrix);
}

void DoubleProjectionScene::update() const
{
    using Core::process_components;

    if (Gfx::has_framebuffer_size_changed())
        update_viewports();

    process_components(control_game_via_keyboard);
    process_components(update_spherical_camera_via_input);
    process_components(update_spherical_camera_view_matrix);
    process_components(read_view_matrix);
    process_components(Gfx::update_perspective_camera_matrix);
    process_components(rotate_in_post_proj_space);
}
} // namespace Game
