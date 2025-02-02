#include <algorithm>

#include "FlyCamera.hpp"
#include "Transformations.hpp"

namespace Age::Gfx
{
FlyCamera::FlyCamera(const Math::Vector3 &position, const Math::Vector2 &rotation, const Math::Vector3 &world_up)
    : _position{position}
    , _world_up{world_up}
{
    add_rotation(rotation);
}

void FlyCamera::add_axial_movement(const Math::Vector3 &axial_movement)
{
    _position += axial_movement.x * _right + axial_movement.y * _world_up + axial_movement.z * _forward;
}

void FlyCamera::add_rotation(const Math::Vector2 &rotation)
{
    _rotation += rotation;
    _rotation.x = std::clamp(_rotation.x, MIN_PITCH, MAX_PITCH);

    float sin_pitch{std::sin(_rotation.x)};
    _forward.x = std::sin(_rotation.y) * sin_pitch;
    _forward.y = std::cos(_rotation.x);
    _forward.z = std::cos(_rotation.y) * sin_pitch;

    _right = Math::normalize(Math::cross(_world_up, _forward));
}

Math::Matrix4 FlyCamera::calc_view_matrix() const
{
    return Math::view_matrix(_position, _position - _forward, _world_up);
}
} // namespace Age::Gfx
