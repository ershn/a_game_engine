#pragma once

#include <string>

#include "Vector.hpp"

namespace Age::Math
{
class Matrix4
{
    Vector4 _columns[4];

  public:
    Matrix4();
    Matrix4(float scalar);
    Matrix4(const Matrix4 &matrix) = default;

    Matrix4 &operator=(const Matrix4 &rhs) = default;

    explicit operator const float *() const;

    const Vector4 &operator[](size_t index) const;
    Vector4 &operator[](size_t index);

    Vector4 row(size_t index) const;

    Matrix4 operator-() const;

    Matrix4 operator*(float scalar) const;

    Matrix4 &operator*=(float scalar);

    Vector4 operator*(const Vector4 &rhs) const;

    Matrix4 operator+(const Matrix4 &rhs) const;

    Matrix4 &operator+=(const Matrix4 &rhs);

    Matrix4 operator-(const Matrix4 &rhs) const;

    Matrix4 &operator-=(const Matrix4 &rhs);

    Matrix4 operator*(const Matrix4 &rhs) const;

    Matrix4 &operator*=(const Matrix4 &rhs);

    Matrix4 &transpose();

    Matrix4 transposed() const;

    std::string to_string() const;
};
} // namespace Age::Math