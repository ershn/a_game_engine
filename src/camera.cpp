#include "camera.hpp"
#include "transformations.hpp"

namespace Age::Gfx
{
namespace
{
float calc_aspect_ratio(const CameraRenderState &camera_render_state)
{
    const Math::Vector2U &framebuffer_size{get_framebuffer_size(camera_render_state.framebuffer_id)};
    const Viewport &viewport{get_viewport(camera_render_state.viewport_id)};
    Math::RectangleI viewport_pixel_rect{calc_viewport_pixel_rect(viewport, framebuffer_size)};
    return static_cast<float>(viewport_pixel_rect.size.x) / viewport_pixel_rect.size.y;
}
} // namespace

bool operator<(CameraStackOrder lhs, CameraStackOrder rhs)
{
    return lhs.stack < rhs.stack || lhs.stack == rhs.stack && lhs.order_in_stack < rhs.order_in_stack;
}

Math::Matrix4 window_space_orthographic_proj_matrix(int viewport_width, int viewport_height)
{
    Math::Matrix4 matrix{};
    matrix[0].x = 2.0f / viewport_width;
    matrix[3].x = -1.0f;
    matrix[1].y = -2.0f / viewport_height;
    matrix[3].y = 1.0f;
    matrix[2].z = 1.0f;
    matrix[3].w = 1.0f;
    return matrix;
}

void update_window_space_orthographic_proj_size(
    Math::Matrix4 &orthographic_matrix, int viewport_width, int viewport_height
)
{
    orthographic_matrix[0].x = 2.0f / viewport_width;
    orthographic_matrix[1].y = -2.0f / viewport_height;
}

void update_perspective_camera_matrix(
    const CameraRenderState &camera_render_state,
    PerspectiveCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionUniformBuffer &projection_uniform_buffer
)
{
    float aspect_ratio{calc_aspect_ratio(camera_render_state)};

    camera.aspect_ratio = aspect_ratio;
    view_to_clip_matrix.matrix =
        Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, aspect_ratio, camera.vertical_fov);

    projection_uniform_buffer.buffer.update({.view_to_clip_matrix{view_to_clip_matrix.matrix}});
}

void update_orthographic_camera_matrix(
    const CameraRenderState &camera_render_state,
    OrthographicCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionUniformBuffer &projection_uniform_buffer
)
{
    float aspect_ratio{calc_aspect_ratio(camera_render_state)};

    camera.aspect_ratio = aspect_ratio;
    view_to_clip_matrix.matrix =
        Math::orthographic_proj_matrix(camera.near_plane_z, camera.far_plane_z, aspect_ratio, camera.vertical_size);

    projection_uniform_buffer.buffer.update({.view_to_clip_matrix{view_to_clip_matrix.matrix}});
}

void update_window_space_camera_matrix(
    const CameraRenderState &camera_render_state,
    WindowSpaceCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionUniformBuffer &projection_uniform_buffer
)
{
    const Math::Vector2U &framebuffer_size{get_framebuffer_size(camera_render_state.framebuffer_id)};
    Math::RectangleI viewport_rect{
        calc_viewport_pixel_rect(get_viewport(camera_render_state.viewport_id), framebuffer_size)
    };

    camera.viewport_width = viewport_rect.size.x;
    camera.viewport_height = viewport_rect.size.y;
    view_to_clip_matrix.matrix = window_space_orthographic_proj_matrix(viewport_rect.size.x, viewport_rect.size.y);

    projection_uniform_buffer.buffer.update({.view_to_clip_matrix{view_to_clip_matrix.matrix}});
}

void calc_camera_view_matrix(const Core::Transform &camera_transform, WorldToViewMatrix &view_matrix)
{
    view_matrix.matrix = Math::affine_rotation_matrix(camera_transform.orientation).transpose() *
                         Math::translation_matrix(-camera_transform.position);
}
} // namespace Age::Gfx
