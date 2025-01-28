#pragma once

#include <iostream>

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

std::ostream &operator<<(std::ostream &out, const Quaternion &quat);

float length(const Quaternion &quat);

Quaternion normalize(const Quaternion &quat);

Quaternion conjugate(const Quaternion &quat);

Quaternion operator*(const Quaternion &a, const Quaternion &b);
Quaternion &operator*=(Quaternion &a, const Quaternion &b);
} // namespace Age::Math
