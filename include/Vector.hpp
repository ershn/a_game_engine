#pragma once

#include <cmath>
#include <iostream>

namespace Age::Math
{
struct Vector3;
struct Vector4;

struct Vector2
{
    float x;
    float y;

    Vector2();
    Vector2(float value);
    Vector2(float x, float y);
    explicit Vector2(const Vector3 &vec);

    explicit operator const float *() const;

    float operator[](size_t index) const;
    float &operator[](size_t index);

    static const Vector2 right;
    static const Vector2 left;
    static const Vector2 up;
    static const Vector2 down;
};

std::ostream &operator<<(std::ostream &out, const Vector2 &vector);

struct Vector3
{
    float x;
    float y;
    float z;

    Vector3();
    Vector3(float value);
    Vector3(float x, float y, float z);
    Vector3(const Vector2 &vec, float z);
    explicit Vector3(const Vector4 &vec);

    explicit operator const float *() const;

    float operator[](size_t index) const;
    float &operator[](size_t index);

    static const Vector3 right;
    static const Vector3 left;
    static const Vector3 up;
    static const Vector3 down;
    static const Vector3 forward;
    static const Vector3 backward;
};

std::ostream &operator<<(std::ostream &out, const Vector3 &vector);

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

    explicit operator const float *() const;

    float operator[](size_t index) const;
    float &operator[](size_t index);

    static const Vector4 right;
    static const Vector4 left;
    static const Vector4 up;
    static const Vector4 down;
    static const Vector4 forward;
    static const Vector4 backward;
};

std::ostream &operator<<(std::ostream &out, const Vector4 &vector);

Vector2 operator-(const Vector2 &vector);
Vector3 operator-(const Vector3 &vector);
Vector4 operator-(const Vector4 &vector);

Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs);
Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs);
Vector4 operator+(const Vector4 &lhs, const Vector4 &rhs);

Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs);
Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs);
Vector4 operator-(const Vector4 &lhs, const Vector4 &rhs);

Vector2 operator*(const Vector2 &vector, float scalar);
Vector2 operator*(float scalar, const Vector2 &vector);
Vector3 operator*(const Vector3 &vector, float scalar);
Vector3 operator*(float scalar, const Vector3 &vector);
Vector4 operator*(const Vector4 &vector, float scalar);
Vector4 operator*(float scalar, const Vector4 &vector);

Vector2 operator/(const Vector2 &vector, float scalar);
Vector3 operator/(const Vector3 &vector, float scalar);
Vector4 operator/(const Vector4 &vector, float scalar);

Vector2 &operator+=(Vector2 &lhs, const Vector2 &rhs);
Vector3 &operator+=(Vector3 &lhs, const Vector3 &rhs);
Vector4 &operator+=(Vector4 &lhs, const Vector4 &rhs);

Vector2 &operator-=(Vector2 &lhs, const Vector2 &rhs);
Vector3 &operator-=(Vector3 &lhs, const Vector3 &rhs);
Vector4 &operator-=(Vector4 &lhs, const Vector4 &rhs);

Vector2 &operator*=(Vector2 &vector, float scalar);
Vector3 &operator*=(Vector3 &vector, float scalar);
Vector4 &operator*=(Vector4 &vector, float scalar);

float dot(const Vector2 &lhs, const Vector2 &rhs);
float dot(const Vector3 &lhs, const Vector3 &rhs);
float dot(const Vector4 &lhs, const Vector4 &rhs);

Vector3 cross(const Vector3 &lhs, const Vector3 &rhs);

template <typename T>
float length(const T &vector)
{
    return std::sqrt(dot(vector, vector));
}

template <typename T>
T normalize(const T &vector)
{
    return vector / length(vector);
}

template <typename T>
float scalar_projection(const T &vector, const T &target)
{
    return dot(vector, target) / length(target);
}

template <typename T>
T vector_projection(const T &vector, const T &target)
{
    return dot(vector, target) / dot(target, target) * target;
}

template <typename T>
T reflect(const T &ray, const T &normal)
{
    return ray - vector_projection(ray, normal) * 2.0f;
}

#define VECTOR2_SWIZZLE(a, b)                                                                                          \
    template <typename T>                                                                                              \
    Vector2 a##b(const T &vector)                                                                                      \
    {                                                                                                                  \
        return Vector2{vector.a, vector.b};                                                                            \
    }

#define VECTOR3_SWIZZLE(a, b, c)                                                                                       \
    template <typename T>                                                                                              \
    Vector3 a##b##c(const T &vector)                                                                                   \
    {                                                                                                                  \
        return Vector3{vector.a, vector.b, vector.c};                                                                  \
    }

#define VECTOR4_SWIZZLE(a, b, c, d)                                                                                    \
    template <typename T>                                                                                              \
    Vector4 a##b##c##d(const T &vector)                                                                                \
    {                                                                                                                  \
        return Vector4{vector.a, vector.b, vector.c, vector.d};                                                        \
    }

#define APPLY_XYZW_ARG1(FUNCTION, ...)                                                                                 \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) x)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) y)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) z)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) w)
#define APPLY_XYZW_ARG2(FUNCTION, ...)                                                                                 \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) x)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) y)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) z)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) w)
#define APPLY_XYZW_ARG3(FUNCTION, ...)                                                                                 \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) x)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) y)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) z)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) w)
#define APPLY_XYZW_ARG4(FUNCTION, ...)                                                                                 \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) x)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) y)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) z)                                                                             \
    FUNCTION(__VA_ARGS__ __VA_OPT__(, ) w)

APPLY_XYZW_ARG1(APPLY_XYZW_ARG2, VECTOR2_SWIZZLE)
APPLY_XYZW_ARG1(APPLY_XYZW_ARG2, APPLY_XYZW_ARG3, VECTOR3_SWIZZLE)
APPLY_XYZW_ARG1(APPLY_XYZW_ARG2, APPLY_XYZW_ARG3, APPLY_XYZW_ARG4, VECTOR4_SWIZZLE)
} // namespace Age::Math
