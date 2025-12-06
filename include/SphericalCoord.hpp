#pragma once

#include "Vector.hpp"

namespace Age::Math
{
struct SphericalCoord
{
    float distance{};
    Vector2 angles{};

    SphericalCoord();
    SphericalCoord(float distance, const Vector2 &angles);
};

Vector3 to_cartesian_coord(const SphericalCoord &spherical_coord);
} // namespace Age::Math
