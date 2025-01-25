#include <functional>

#include "Camera.hpp"
#include "Transformations.hpp"
#include "UniformBufferInstances.hpp"

namespace Age::Gfx
{
namespace
{
void update_camera_aspect_ratio(float aspect_ratio,
                                Camera &camera,
                                CameraToClipMatrix &camera_to_clip_matrix,
                                const ProjectionBufferRef &projection_buffer_ref)
{
    camera.aspect_ratio = aspect_ratio;
    Math::update_fov(camera_to_clip_matrix.matrix, aspect_ratio, camera.vertical_fov);

    const auto &projection_buffer =
        static_cast<const ProjectionUniformBuffer &>(get_uniform_buffer(projection_buffer_ref.buffer_id));
    projection_buffer.set_camera_to_clip_matrix(camera_to_clip_matrix.matrix);
}
} // namespace

void update_cameras_aspect_ratio(float aspect_ratio)
{
    Core::process_components(std::function<void(Camera &, CameraToClipMatrix &, const ProjectionBufferRef &)>{
        std::bind_front(update_camera_aspect_ratio, aspect_ratio)});
}
} // namespace Age::Gfx
