#include "SphericalCoord.hpp"

namespace Age::Math
{
SphericalCoord::SphericalCoord()
    : angles{}
    , distance{}
{
}

SphericalCoord::SphericalCoord(const Vector2 &angles, float distance)
    : angles{angles}
    , distance{distance}
{
}

Vector3 to_cartesian_coord(const SphericalCoord &spherical_coord)
{
    float r{std::sin(spherical_coord.angles.x) * spherical_coord.distance};
    return Vector3{std::sin(spherical_coord.angles.y) * r,
                   std::cos(spherical_coord.angles.x) * spherical_coord.distance,
                   std::cos(spherical_coord.angles.y) * r};
}
} // namespace Age::Math
