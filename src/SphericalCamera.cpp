#include <algorithm>

#include "SphericalCamera.hpp"
#include "Transformations.hpp"

namespace Age::Gfx
{
void update_spherical_camera_via_input(const Input::MouseInput &mouse_input, SphericalCamera &spherical_camera)
{
    Math::SphericalCoord &spherical_coord{spherical_camera.spherical_coord};

    Math::Vector2 motion_delta{Input::get_cursor_position_delta(mouse_input)};
    spherical_coord.angles += Math::Vector2{motion_delta.y, motion_delta.x};
    spherical_coord.angles.x =
        std::clamp(spherical_coord.angles.x, spherical_camera.min_pitch, spherical_camera.max_pitch);

    float scroll_delta{-Input::get_scroll_delta(mouse_input).y};
    spherical_coord.distance = std::clamp(
        spherical_coord.distance + scroll_delta, spherical_camera.min_distance, spherical_camera.max_distance
    );
}

void calc_spherical_camera_view_matrix(const SphericalCamera &spherical_camera, WorldToViewMatrix &view_matrix)
{
    view_matrix.matrix = Math::look_at_matrix(spherical_camera.origin, spherical_camera.spherical_coord);
}
} // namespace Age::Gfx
