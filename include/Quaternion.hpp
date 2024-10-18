#pragma once

#include "Vector.hpp"

namespace Age::Math
{
struct Quaternion
{
    float w;
    float x;
    float y;
    float z;

    Quaternion();
    Quaternion(float w, float x, float y, float z);
};

float length(const Quaternion &quat);

Quaternion normalize(const Quaternion &quat);

Quaternion conjugate(const Quaternion &quat);

Quaternion operator*(const Quaternion &a, const Quaternion &b);
} // namespace Age::Math
