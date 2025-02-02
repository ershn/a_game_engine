#include <functional>

#include "Camera.hpp"
#include "Transformations.hpp"

namespace Age::Gfx
{
namespace
{
void update_camera_aspect_ratio(float aspect_ratio,
                                Camera &camera,
                                CameraToClipMatrix &camera_to_clip_matrix,
                                const ProjectionBufferBlock &projection_buffer_block)
{
    camera.aspect_ratio = aspect_ratio;
    Math::update_fov(camera_to_clip_matrix.matrix, aspect_ratio, camera.vertical_fov);

    projection_buffer_block = {.view_to_clip_matrix{camera_to_clip_matrix.matrix}};
}
} // namespace

void update_cameras_aspect_ratio(float aspect_ratio)
{
    Core::process_components(std::function<void(Camera &, CameraToClipMatrix &, const ProjectionBufferBlock &)>{
        std::bind_front(update_camera_aspect_ratio, aspect_ratio)});
}
} // namespace Age::Gfx
