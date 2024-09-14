#pragma once

#include <string>

#include "Vector.hpp"

namespace Age::Math
{
float determinant(const Vector2 &col1, const Vector2 &col2);
float determinant(const Vector3 &col1, const Vector3 &col2, const Vector3 &col3);
float determinant(const Vector4 &col1, const Vector4 &col2, const Vector4 &col3,
                  const Vector4 &col4);

class Matrix3
{
    Vector3 _columns[3];

  public:
    Matrix3();
    Matrix3(float scalar);
    Matrix3(const Vector3 &col1, const Vector3 &col2, const Vector3 &col3);
    Matrix3(const Matrix3 &matrix) = default;

    Matrix3 &operator=(const Matrix3 &rhs) = default;

    explicit operator const float *() const;

    const Vector3 &operator[](size_t index) const;
    Vector3 &operator[](size_t index);

    Vector3 row(size_t index) const;

    Matrix3 operator-() const;

    Matrix3 operator*(float scalar) const;

    Matrix3 &operator*=(float scalar);

    Vector3 operator*(const Vector3 &rhs) const;

    Matrix3 operator+(const Matrix3 &rhs) const;

    Matrix3 &operator+=(const Matrix3 &rhs);

    Matrix3 operator-(const Matrix3 &rhs) const;

    Matrix3 &operator-=(const Matrix3 &rhs);

    Matrix3 operator*(const Matrix3 &rhs) const;

    Matrix3 &operator*=(const Matrix3 &rhs);

    Matrix3 &transpose();

    Matrix3 transposed() const;

    float determinant() const;

    Matrix3 &invert();

    Matrix3 inverted() const;

    std::string to_string() const;
};

class Matrix4
{
    Vector4 _columns[4];

  public:
    Matrix4();
    Matrix4(float scalar);
    Matrix4(const Vector4 &col1, const Vector4 &col2, const Vector4 &col3, const Vector4 &col4);
    Matrix4(const Matrix4 &matrix) = default;

    Matrix4 &operator=(const Matrix4 &rhs) = default;

    explicit operator const float *() const;

    const Vector4 &operator[](size_t index) const;
    Vector4 &operator[](size_t index);

    Matrix3 to_matrix3() const;

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

    float determinant() const;

    Matrix4 &invert();

    Matrix4 inverted() const;

    std::string to_string() const;
};
} // namespace Age::Math
