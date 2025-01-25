#pragma once

#include "Vector.hpp"

namespace Age::Math
{
struct SphericalCoord
{
    Vector2 angles{};
    float distance{};

    SphericalCoord();
    SphericalCoord(const Vector2 &angles, float distance);
};

Vector3 to_cartesian_coord(const SphericalCoord &spherical_coord);
} // namespace Age::Math
