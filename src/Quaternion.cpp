#include <cmath>

#include "Quaternion.hpp"

namespace Age::Math
{
Quaternion::Quaternion()
    : w{}
    , x{}
    , y{}
    , z{}
{
}

Quaternion::Quaternion(float w, float x, float y, float z)
    : w{w}
    , x{x}
    , y{y}
    , z{z}
{
}

float length(const Quaternion &quat)
{
    return std::sqrt(quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z);
}

Quaternion normalize(const Quaternion &quat)
{
    float length_reciprocal{1.0f / length(quat)};
    return Quaternion{quat.w * length_reciprocal, quat.x * length_reciprocal,
                      quat.y * length_reciprocal, quat.z * length_reciprocal};
}

Quaternion conjugate(const Quaternion &quat)
{
    return Quaternion{quat.w, -quat.x, -quat.y, -quat.z};
}

Quaternion operator*(const Quaternion &a, const Quaternion &b)
{
    return Quaternion{a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
                      a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                      a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
                      a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x};
}
} // namespace Age::Math
