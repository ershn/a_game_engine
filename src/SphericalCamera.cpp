#include <algorithm>

#include "SphericalCamera.hpp"
#include "Transformations.hpp"

namespace Age::Gfx
{
SphericalCamera::SphericalCamera(const Math::Vector3 &origin, const Math::Vector2 &angles,
                                 float distance, const Math::Vector3 &world_up)
    : _origin{origin}
    , _angles{angles}
    , _distance{distance}
    , _world_up{world_up}
{
    _angles.x = std::clamp(_angles.x, MIN_PITCH, MAX_PITCH);

    update_position();
}

void SphericalCamera::add_angles(const Math::Vector2 &angles)
{
    _angles += angles;
    _angles.x = std::clamp(_angles.x, MIN_PITCH, MAX_PITCH);

    update_position();
}

void SphericalCamera::add_distance(float distance)
{
    _distance += distance;

    update_position();
}

Math::Matrix4 SphericalCamera::calc_camera_matrix() const
{
    return Math::camera_matrix(_origin + _position, _origin, _world_up);
}

void SphericalCamera::update_position()
{
    float r{std::sin(_angles.x) * _distance};
    _position.x = std::sin(_angles.y) * r;
    _position.y = std::cos(_angles.x) * _distance;
    _position.z = std::cos(_angles.y) * r;
}
} // namespace Age::Gfx
