#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <type_traits>

#include "error_handling.hpp"

namespace Age::Math
{
template <typename T>
struct Vector2X
{
    using value_type = T;

    T x;
    T y;

    constexpr Vector2X() = default;

    constexpr Vector2X(T value)
        : x{value}
        , y{value}
    {
    }

    constexpr Vector2X(T x, T y)
        : x{x}
        , y{y}
    {
    }

    template <typename U>
    explicit constexpr Vector2X(const Vector2X<U> &vector)
        : x{static_cast<T>(vector.x)}
        , y{static_cast<T>(vector.y)}
    {
    }

    constexpr bool operator==(const Vector2X &rhs) const = default;

    explicit operator const T *() const
    {
        return &x;
    }

    T operator[](std::size_t index) const
    {
        LOG_ERROR_IF(index > 1, "Vector2X index must be in the range [0, 1]: {}", index);

        return *(&x + index % 2);
    }

    T &operator[](std::size_t index)
    {
        LOG_ERROR_IF(index > 1, "Vector2X index must be in the range [0, 1]: {}", index);

        return *(&x + index % 2);
    }
};

using Vector2 = Vector2X<float>;
using Vector2I = Vector2X<int>;
using Vector2U = Vector2X<unsigned int>;

std::ostream &operator<<(std::ostream &out, const Vector2 &vector);
std::ostream &operator<<(std::ostream &out, const Vector2I &vector);
std::ostream &operator<<(std::ostream &out, const Vector2U &vector);

template <typename T>
struct Vector3X
{
    using value_type = T;

    T x;
    T y;
    T z;

    constexpr Vector3X() = default;

    constexpr Vector3X(T value)
        : x{value}
        , y{value}
        , z{value}
    {
    }

    constexpr Vector3X(T x, T y, T z)
        : x{x}
        , y{y}
        , z{z}
    {
    }

    constexpr Vector3X(const Vector2X<T> &vector, T z)
        : x{vector.x}
        , y{vector.y}
        , z{z}
    {
    }

    template <typename U>
    explicit constexpr Vector3X(const Vector3X<U> &vector)
        : x{static_cast<T>(vector.x)}
        , y{static_cast<T>(vector.y)}
        , z{static_cast<T>(vector.z)}
    {
    }

    constexpr bool operator==(const Vector3X<T> &rhs) const = default;

    explicit operator const T *() const
    {
        return &x;
    }

    T operator[](std::size_t index) const
    {
        LOG_ERROR_IF(index > 2, "Vector3X index must be in the range [0, 2]: {}", index);

        return *(&x + index % 3);
    }

    T &operator[](std::size_t index)
    {
        LOG_ERROR_IF(index > 2, "Vector3X index must be in the range [0, 2]: {}", index);

        return *(&x + index % 3);
    }
};

using Vector3 = Vector3X<float>;
using Vector3I = Vector3X<int>;
using Vector3U = Vector3X<unsigned int>;

std::ostream &operator<<(std::ostream &out, const Vector3 &vector);
std::ostream &operator<<(std::ostream &out, const Vector3I &vector);
std::ostream &operator<<(std::ostream &out, const Vector3U &vector);

template <typename T>
struct Vector4X
{
    using value_type = T;

    T x;
    T y;
    T z;
    T w;

    constexpr Vector4X() = default;

    constexpr Vector4X(T value)
        : x{value}
        , y{value}
        , z{value}
        , w{value}
    {
    }

    constexpr Vector4X(T x, T y, T z, T w)
        : x{x}
        , y{y}
        , z{z}
        , w{w}
    {
    }

    constexpr Vector4X(const Vector3X<T> &vector, T w)
        : x{vector.x}
        , y{vector.y}
        , z{vector.z}
        , w{w}
    {
    }

    template <typename U>
    explicit constexpr Vector4X(const Vector4X<U> &vector)
        : x{static_cast<T>(vector.x)}
        , y{static_cast<T>(vector.y)}
        , z{static_cast<T>(vector.z)}
        , w{static_cast<T>(vector.w)}
    {
    }

    constexpr bool operator==(const Vector4X<T> &rhs) const = default;

    explicit operator const T *() const
    {
        return &x;
    }

    T operator[](std::size_t index) const
    {
        LOG_ERROR_IF(index > 3, "Vector4X index must be in the range [0, 3]: {}", index);

        return *(&x + index % 4);
    }

    T &operator[](std::size_t index)
    {
        LOG_ERROR_IF(index > 3, "Vector4X index must be in the range [0, 3]: {}", index);

        return *(&x + index % 4);
    }
};

using Vector4 = Vector4X<float>;
using Vector4I = Vector4X<int>;
using Vector4U = Vector4X<unsigned int>;

std::ostream &operator<<(std::ostream &out, const Vector4 &vector);
std::ostream &operator<<(std::ostream &out, const Vector4I &vector);
std::ostream &operator<<(std::ostream &out, const Vector4U &vector);

template <typename T>
constexpr T min()
{
    return {std::numeric_limits<typename T::value_type>::min()};
};

template <typename T>
constexpr T max()
{
    return {std::numeric_limits<typename T::value_type>::max()};
};

template <typename T>
T right;

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector2X<T> right<Vector2X<T>>{1, 0};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector3X<T> right<Vector3X<T>>{1, 0, 0};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector4X<T> right<Vector4X<T>>{1, 0, 0, 1};

template <typename T>
T left;

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector2X<T> left<Vector2X<T>>{-1, 0};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector3X<T> left<Vector3X<T>>{-1, 0, 0};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector4X<T> left<Vector4X<T>>{-1, 0, 0, 1};

template <typename T>
T up;

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector2X<T> up<Vector2X<T>>{0, 1};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector3X<T> up<Vector3X<T>>{0, 1, 0};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector4X<T> up<Vector4X<T>>{0, 1, 0, 1};

template <typename T>
T down;

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector2X<T> down<Vector2X<T>>{0, -1};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector3X<T> down<Vector3X<T>>{0, -1, 0};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector4X<T> down<Vector4X<T>>{0, -1, 0, 1};

template <typename T>
T forward;

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector3X<T> forward<Vector3X<T>>{0, 0, -1};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector4X<T> forward<Vector4X<T>>{0, 0, -1, 1};

template <typename T>
T backward;

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector3X<T> backward<Vector3X<T>>{0, 0, 1};

template <typename T>
    requires std::is_signed_v<T>
inline constexpr Vector4X<T> backward<Vector4X<T>>{0, 0, 1, 1};

template <typename T>
    requires std::is_signed_v<T>
constexpr Vector2X<T> operator-(const Vector2X<T> &vector)
{
    return {-vector.x, -vector.y};
}

template <typename T>
    requires std::is_signed_v<T>
constexpr Vector3X<T> operator-(const Vector3X<T> &vector)
{
    return {-vector.x, -vector.y, -vector.z};
}

template <typename T>
    requires std::is_signed_v<T>
constexpr Vector4X<T> operator-(const Vector4X<T> &vector)
{
    return {-vector.x, -vector.y, -vector.z, -vector.w};
}

template <typename T>
constexpr Vector2X<T> operator+(const Vector2X<T> &lhs, const Vector2X<T> &rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

template <typename T>
constexpr Vector3X<T> operator+(const Vector3X<T> &lhs, const Vector3X<T> &rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

template <typename T>
constexpr Vector4X<T> operator+(const Vector4X<T> &lhs, const Vector4X<T> &rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
}

template <typename T>
constexpr Vector2X<T> operator-(const Vector2X<T> &lhs, const Vector2X<T> &rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

template <typename T>
constexpr Vector3X<T> operator-(const Vector3X<T> &lhs, const Vector3X<T> &rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

template <typename T>
constexpr Vector4X<T> operator-(const Vector4X<T> &lhs, const Vector4X<T> &rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
}

template <typename T>
constexpr Vector2X<T> operator*(const Vector2X<T> &vector, T scalar)
{
    return {vector.x * scalar, vector.y * scalar};
}

template <typename T>
constexpr Vector2X<T> operator*(T scalar, const Vector2X<T> &vector)
{
    return {vector.x * scalar, vector.y * scalar};
}

template <typename T>
constexpr Vector3X<T> operator*(const Vector3X<T> &vector, T scalar)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

template <typename T>
constexpr Vector3X<T> operator*(T scalar, const Vector3X<T> &vector)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

template <typename T>
constexpr Vector4X<T> operator*(const Vector4X<T> &vector, T scalar)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar};
}

template <typename T>
constexpr Vector4X<T> operator*(T scalar, const Vector4X<T> &vector)
{
    return {vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar};
}

template <typename T>
constexpr Vector2X<T> operator/(const Vector2X<T> &vector, T scalar)
{
    return {vector.x / scalar, vector.y / scalar};
}

template <typename T>
constexpr Vector3X<T> operator/(const Vector3X<T> &vector, T scalar)
{
    return {vector.x / scalar, vector.y / scalar, vector.z / scalar};
}

template <typename T>
constexpr Vector4X<T> operator/(const Vector4X<T> &vector, T scalar)
{
    return {vector.x / scalar, vector.y / scalar, vector.z / scalar, vector.w / scalar};
}

template <typename T>
constexpr Vector2X<T> &operator+=(Vector2X<T> &lhs, const Vector2X<T> &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

template <typename T>
constexpr Vector3X<T> &operator+=(Vector3X<T> &lhs, const Vector3X<T> &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

template <typename T>
constexpr Vector4X<T> &operator+=(Vector4X<T> &lhs, const Vector4X<T> &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
    return lhs;
}

template <typename T>
constexpr Vector2X<T> &operator-=(Vector2X<T> &lhs, const Vector2X<T> &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

template <typename T>
constexpr Vector3X<T> &operator-=(Vector3X<T> &lhs, const Vector3X<T> &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

template <typename T>
constexpr Vector4X<T> &operator-=(Vector4X<T> &lhs, const Vector4X<T> &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
    return lhs;
}

template <typename T>
constexpr Vector2X<T> &operator*=(Vector2X<T> &lhs, T scalar)
{
    lhs.x *= scalar;
    lhs.y *= scalar;
    return lhs;
}

template <typename T>
constexpr Vector3X<T> &operator*=(Vector3X<T> &lhs, T scalar)
{
    lhs.x *= scalar;
    lhs.y *= scalar;
    lhs.z *= scalar;
    return lhs;
}

template <typename T>
constexpr Vector4X<T> &operator*=(Vector4X<T> &lhs, T scalar)
{
    lhs.x *= scalar;
    lhs.y *= scalar;
    lhs.z *= scalar;
    lhs.w *= scalar;
    return lhs;
}

template <typename T>
constexpr Vector2X<T> &operator/=(Vector2X<T> &lhs, T scalar)
{
    lhs.x /= scalar;
    lhs.y /= scalar;
    return lhs;
}

template <typename T>
constexpr Vector3X<T> &operator/=(Vector3X<T> &lhs, T scalar)
{
    lhs.x /= scalar;
    lhs.y /= scalar;
    lhs.z /= scalar;
    return lhs;
}

template <typename T>
constexpr Vector4X<T> &operator/=(Vector4X<T> &lhs, T scalar)
{
    lhs.x /= scalar;
    lhs.y /= scalar;
    lhs.z /= scalar;
    lhs.w /= scalar;
    return lhs;
}

template <typename T>
constexpr Vector2X<T> min(const Vector2X<T> &a, const Vector2X<T> &b)
{
    return {std::min(a.x, b.x), std::min(a.y, b.y)};
}

template <typename T>
constexpr Vector3X<T> min(const Vector3X<T> &a, const Vector3X<T> &b)
{
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}

template <typename T>
constexpr Vector4X<T> min(const Vector4X<T> &a, const Vector4X<T> &b)
{
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)};
}

template <typename T>
constexpr Vector2X<T> max(const Vector2X<T> &a, const Vector2X<T> &b)
{
    return {std::max(a.x, b.x), std::max(a.y, b.y)};
}

template <typename T>
constexpr Vector3X<T> max(const Vector3X<T> &a, const Vector3X<T> &b)
{
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}

template <typename T>
constexpr Vector4X<T> max(const Vector4X<T> &a, const Vector4X<T> &b)
{
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)};
}

Vector2 pow(const Vector2 &base, const Vector2 &exponent);
Vector3 pow(const Vector3 &base, const Vector3 &exponent);
Vector4 pow(const Vector4 &base, const Vector4 &exponent);

constexpr float dot(const Vector2 &lhs, const Vector2 &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

constexpr float dot(const Vector3 &lhs, const Vector3 &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

constexpr float dot(const Vector4 &lhs, const Vector4 &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

constexpr Vector3 cross(const Vector3 &lhs, const Vector3 &rhs)
{
    return {lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x};
}

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
constexpr T vector_projection(const T &vector, const T &target)
{
    return dot(vector, target) / dot(target, target) * target;
}

template <typename T>
constexpr T reflect(const T &ray, const T &normal)
{
    return ray - vector_projection(ray, normal) * 2.0f;
}

#define VECTOR2_SWIZZLE(a, b)                                                                                          \
    template <template <typename> typename V, typename T>                                                              \
    constexpr Vector2X<T> a##b(const V<T> &vector)                                                                     \
    {                                                                                                                  \
        return {vector.a, vector.b};                                                                                   \
    }

#define VECTOR3_SWIZZLE(a, b, c)                                                                                       \
    template <template <typename> typename V, typename T>                                                              \
    constexpr Vector3X<T> a##b##c(const V<T> &vector)                                                                  \
    {                                                                                                                  \
        return {vector.a, vector.b, vector.c};                                                                         \
    }

#define VECTOR4_SWIZZLE(a, b, c, d)                                                                                    \
    template <template <typename> typename V, typename T>                                                              \
    constexpr Vector4X<T> a##b##c##d(const V<T> &vector)                                                               \
    {                                                                                                                  \
        return {vector.a, vector.b, vector.c, vector.d};                                                               \
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
