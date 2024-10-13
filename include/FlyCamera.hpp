#pragma once

#include "Math.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
class FlyCamera
{
    const float MIN_PITCH{Math::radians(0.1f)};
    const float MAX_PITCH{Math::radians(179.9f)};

    Math::Vector3 _position{};
    Math::Vector2 _rotation{};

    Math::Vector3 _forward{};
    Math::Vector3 _right{};
    const Math::Vector3 _world_up{};

  public:
    FlyCamera(const Math::Vector3 &position, const Math::Vector2 &rotation,
              const Math::Vector3 &world_up);

    void add_axial_movement(const Math::Vector3 &axial_movement);
    void add_rotation(const Math::Vector2 &rotation);

    Math::Matrix4 calc_camera_matrix() const;
};
} // namespace Age::Gfx
