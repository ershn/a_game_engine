#include "Camera.hpp"
#include "Transformations.hpp"

namespace Age::Gfx
{
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
    const Viewport &viewport{get_viewport(camera_render_state.viewport_id)};
    float aspect_ratio{static_cast<float>(viewport.width) / viewport.height};

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
    const Viewport &viewport{get_viewport(camera_render_state.viewport_id)};
    float aspect_ratio{static_cast<float>(viewport.width) / viewport.height};

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
    const Viewport &viewport{get_viewport(camera_render_state.viewport_id)};

    camera.viewport_width = viewport.width;
    camera.viewport_height = viewport.height;
    view_to_clip_matrix.matrix = window_space_orthographic_proj_matrix(viewport.width, viewport.height);

    projection_uniform_buffer.buffer.update({.view_to_clip_matrix{view_to_clip_matrix.matrix}});
}

void calc_camera_view_matrix(const Core::Transform &camera_transform, WorldToViewMatrix &view_matrix)
{
    view_matrix.matrix = Math::affine_rotation_matrix(camera_transform.orientation).transpose() *
                         Math::translation_matrix(-camera_transform.position);
}
} // namespace Age::Gfx
