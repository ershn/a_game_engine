#pragma once

#include "Math.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
class SphericalCamera
{
    const float MIN_PITCH{Math::radians(0.1f)};
    const float MAX_PITCH{Math::radians(179.9f)};

    Math::Vector3 _origin{};
    Math::Vector2 _angles{};
    float _distance{};
    Math::Vector3 _position{};

    const Math::Vector3 _world_up{};

  public:
    SphericalCamera(const Math::Vector3 &origin, const Math::Vector2 &angles, float distance,
                    const Math::Vector3 &world_up);

    void add_angles(const Math::Vector2 &angles);
    void add_distance(float distance);

    Math::Matrix4 calc_camera_matrix() const;

  private:
    void update_position();
};
} // namespace Age::Gfx
