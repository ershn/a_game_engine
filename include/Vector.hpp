#pragma once

#include <string>

namespace Age::Math
{
struct Vector2
{
    float x;
    float y;

    Vector2();
    Vector2(float value);
    Vector2(float x, float y);
    Vector2(const Vector2 &vec) = default;

    explicit operator const float *() const;

    float operator[](size_t index) const;
    float &operator[](size_t index);

    float length() const;

    Vector2 &normalize();

    Vector2 normalized() const;

    Vector2 operator-() const;

    Vector2 operator*(float scalar) const;

    Vector2 &operator*=(float scalar);

    Vector2 operator+(const Vector2 &rhs) const;

    Vector2 &operator+=(const Vector2 &rhs);

    Vector2 operator-(const Vector2 &rhs) const;

    Vector2 &operator-=(const Vector2 &rhs);

    float dot(const Vector2 &rhs) const;

    std::string to_string() const;
};

struct Vector3
{
    float x;
    float y;
    float z;

    Vector3();
    Vector3(float value);
    Vector3(float x, float y, float z);
    Vector3(const Vector2 &vec, float z);
    Vector3(const Vector3 &vec) = default;

    explicit operator const float *() const;

    float operator[](size_t index) const;
    float &operator[](size_t index);

    Vector2 xy() const;
    Vector2 xz() const;
    Vector2 yz() const;

    float length() const;

    Vector3 &normalize();

    Vector3 normalized() const;

    Vector3 operator-() const;

    Vector3 operator*(float scalar) const;

    Vector3 &operator*=(float scalar);

    Vector3 operator+(const Vector3 &rhs) const;

    Vector3 &operator+=(const Vector3 &rhs);

    Vector3 operator-(const Vector3 &rhs) const;

    Vector3 &operator-=(const Vector3 &rhs);

    float dot(const Vector3 &rhs) const;

    Vector3 cross(const Vector3 &rhs) const;

    std::string to_string() const;
};

struct Vector4
{
    float x;
    float y;
    float z;
    float w;

    Vector4();
    Vector4(float value);
    Vector4(float x, float y, float z, float w);
    Vector4(const Vector3 &vec, float w);
    Vector4(const Vector4 &vec) = default;

    explicit operator const float *() const;

    float operator[](size_t index) const;
    float &operator[](size_t index);

    Vector2 xy() const;
    Vector2 xz() const;
    Vector2 xw() const;
    Vector2 yz() const;
    Vector2 yw() const;
    Vector2 zw() const;

    Vector3 xyz() const;
    Vector3 xyw() const;
    Vector3 xzw() const;
    Vector3 yzw() const;

    float length() const;

    Vector4 &normalize();

    Vector4 normalized() const;

    Vector4 operator-() const;

    Vector4 operator*(float scalar) const;

    Vector4 &operator*=(float scalar);

    Vector4 operator+(const Vector4 &rhs) const;

    Vector4 &operator+=(const Vector4 &rhs);

    Vector4 operator-(const Vector4 &rhs) const;

    Vector4 &operator-=(const Vector4 &rhs);

    float dot(const Vector4 &rhs) const;

    std::string to_string() const;
};
} // namespace Age::Math