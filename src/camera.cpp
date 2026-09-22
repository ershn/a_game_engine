#include "camera.hpp"
#include "transformations.hpp"

namespace Age::Gfx
{
namespace
{
float calc_aspect_ratio(const CameraRenderState &camera_render_state)
{
    const Math::Vector2U &framebuffer_size{get_framebuffer_size(camera_render_state.framebuffer_id)};
    Math::Vector2I viewport_size{mapped_viewport_size(camera_render_state.viewport_id, framebuffer_size)};
    return static_cast<float>(viewport_size.x) / viewport_size.y;
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
    Math::Vector2I viewport_size{mapped_viewport_size(camera_render_state.viewport_id, framebuffer_size)};

    camera.viewport_width = viewport_size.x;
    camera.viewport_height = viewport_size.y;
    view_to_clip_matrix.matrix = window_space_orthographic_proj_matrix(viewport_size.x, viewport_size.y);

    projection_uniform_buffer.buffer.update({.view_to_clip_matrix{view_to_clip_matrix.matrix}});
}

void calc_camera_view_matrix(const Core::Transform &camera_transform, WorldToViewMatrix &view_matrix)
{
    view_matrix.matrix = Math::transpose(Math::affine_rotation_matrix(camera_transform.orientation)) *
                         Math::translation_matrix(-camera_transform.position);
}

MappedViewport mapped_camera_viewport(const CameraRenderState &camera_render_state)
{
    return mapped_viewport(camera_render_state.viewport_id, get_framebuffer_size(camera_render_state.framebuffer_id));
}
} // namespace Age::Gfx
