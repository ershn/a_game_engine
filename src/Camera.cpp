#include <functional>

#include "Camera.hpp"
#include "Transformations.hpp"

namespace Age::Gfx
{
namespace
{
void update_perspective_camera_aspect_ratio(
    float aspect_ratio,
    PerspectiveCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionBufferBlockRef &projection_buffer_block
)
{
    camera.aspect_ratio = aspect_ratio;
    Math::update_perspective_proj_zoom(view_to_clip_matrix.matrix, aspect_ratio, camera.vertical_fov);

    projection_buffer_block = {.view_to_clip_matrix{view_to_clip_matrix.matrix}};
}

void update_orthographic_camera_aspect_ratio(
    float aspect_ratio,
    OrthographicCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionBufferBlockRef &projection_buffer_block
)
{
    camera.aspect_ratio = aspect_ratio;
    Math::update_orthographic_proj_size(view_to_clip_matrix.matrix, aspect_ratio, camera.vertical_size);

    projection_buffer_block = {.view_to_clip_matrix{view_to_clip_matrix.matrix}};
}

void update_window_space_camera(
    int viewport_width,
    int viewport_height,
    WindowSpaceCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionBufferBlockRef &projection_buffer_block
)
{
    camera.viewport_width = viewport_width;
    camera.viewport_height = viewport_height;
    update_window_space_orthographic_proj_size(view_to_clip_matrix.matrix, viewport_width, viewport_height);

    projection_buffer_block = {.view_to_clip_matrix{view_to_clip_matrix.matrix}};
}
} // namespace

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

void calc_camera_view_matrix(const Core::Transform &camera_transform, WorldToViewMatrix &view_matrix)
{
    view_matrix.matrix = Math::affine_rotation_matrix(camera_transform.orientation).transpose() *
                         Math::translation_matrix(-camera_transform.position);
}

void update_camera_projections(int viewport_width, int viewport_height)
{
    float aspect_ratio{static_cast<float>(viewport_width) / viewport_height};

    Core::process_components(
        std::function<void(PerspectiveCamera &, ViewToClipMatrix &, const ProjectionBufferBlockRef &)>{
            std::bind_front(update_perspective_camera_aspect_ratio, aspect_ratio)
        }
    );
    Core::process_components(
        std::function<void(OrthographicCamera &, ViewToClipMatrix &, const ProjectionBufferBlockRef &)>{
            std::bind_front(update_orthographic_camera_aspect_ratio, aspect_ratio)
        }
    );
    Core::process_components(
        std::function<void(WindowSpaceCamera &, ViewToClipMatrix &, const ProjectionBufferBlockRef &)>{
            std::bind_front(update_window_space_camera, viewport_width, viewport_height)
        }
    );
}
} // namespace Age::Gfx
