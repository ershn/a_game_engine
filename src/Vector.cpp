#include <sstream>

#include "Vector.hpp"

namespace Age::Math
{
Vector2::Vector2()
    : x{}
    , y{}
{
}

Vector2::Vector2(float value)
    : x{value}
    , y{value}
{
}

Vector2::Vector2(float x, float y)
    : x{x}
    , y{y}
{
}

Vector2::Vector2(const Vector3 &vec)
    : x{vec.x}
    , y{vec.y}
{
}

Vector2::operator const float *() const
{
    return &x;
}

float Vector2::operator[](size_t index) const
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    default:
        throw std::out_of_range("Index must be in [0, 1]");
    }
}

float &Vector2::operator[](size_t index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    default:
        throw std::out_of_range("Index must be in [0, 1]");
    }
}

const Vector2 Vector2::right{1.0f, 0.0f};
const Vector2 Vector2::left{-1.0f, 0.0f};
const Vector2 Vector2::up{0.0f, 1.0f};
const Vector2 Vector2::down{0.0f, -1.0f};

std::ostream &operator<<(std::ostream &out, const Vector2 &vector)
{
    out << std::showpoint << '{' << vector.x << "f, " << vector.y << "f}";
    return out;
}

Vector3::Vector3()
    : x{}
    , y{}
    , z{}
{
}

Vector3::Vector3(float value)
    : x{value}
    , y{value}
    , z{value}
{
}

Vector3::Vector3(float x, float y, float z)
    : x{x}
    , y{y}
    , z{z}
{
}

Vector3::Vector3(const Vector2 &vec, float z)
    : x{vec.x}
    , y{vec.y}
    , z{z}
{
}

Vector3::Vector3(const Vector4 &vec)
    : x{vec.x}
    , y{vec.y}
    , z{vec.z}
{
}

Vector3::operator const float *() const
{
    return &x;
}

float Vector3::operator[](size_t index) const
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        throw std::out_of_range("Index must be in [0, 2]");
    }
}

float &Vector3::operator[](size_t index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        throw std::out_of_range("Index must be in [0, 2]");
    }
}

const Vector3 Vector3::right{1.0f, 0.0f, 0.0f};
const Vector3 Vector3::left{-1.0f, 0.0f, 0.0f};
const Vector3 Vector3::up{0.0f, 1.0f, 0.0f};
const Vector3 Vector3::down{0.0f, -1.0f, 0.0f};
const Vector3 Vector3::forward{0.0f, 0.0f, -1.0f};
const Vector3 Vector3::backward{0.0f, 0.0f, 1.0f};

std::ostream &operator<<(std::ostream &out, const Vector3 &vector)
{
    out << std::showpoint << '{' << vector.x << "f, " << vector.y << "f, " << vector.z << "f}";
    return out;
}

Vector4::Vector4()
    : x{}
    , y{}
    , z{}
    , w{}
{
}

Vector4::Vector4(float value)
    : x{value}
    , y{value}
    , z{value}
    , w{value}
{
}

Vector4::Vector4(float x, float y, float z, float w)
    : x{x}
    , y{y}
    , z{z}
    , w{w}
{
}

Vector4::Vector4(const Vector3 &vec, float w)
    : x{vec.x}
    , y{vec.y}
    , z{vec.z}
    , w{w}
{
}

Vector4::operator const float *() const
{
    return &x;
}

float Vector4::operator[](size_t index) const
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        throw std::out_of_range("Index must be in [0, 3]");
    }
}

float &Vector4::operator[](size_t index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        throw std::out_of_range("Index must be in [0, 3]");
    }
}

const Vector4 Vector4::right{1.0f, 0.0f, 0.0f, 1.0f};
const Vector4 Vector4::left{-1.0f, 0.0f, 0.0f, 1.0f};
const Vector4 Vector4::up{0.0f, 1.0f, 0.0f, 1.0f};
const Vector4 Vector4::down{0.0f, -1.0f, 0.0f, 1.0f};
const Vector4 Vector4::forward{0.0f, 0.0f, -1.0f, 1.0f};
const Vector4 Vector4::backward{0.0f, 0.0f, 1.0f, 1.0f};

std::ostream &operator<<(std::ostream &out, const Vector4 &vector)
{
    out << std::showpoint << '{' << vector.x << "f, " << vector.y << "f, " << vector.z << "f, " << vector.w << "f}";
    return out;
}

Vector2 operator-(const Vector2 &vector)
{
    return Vector2{-vector.x, -vector.y};
}

Vector3 operator-(const Vector3 &vector)
{
    return Vector3{-vector.x, -vector.y, -vector.z};
}

Vector4 operator-(const Vector4 &vector)
{
    return Vector4{-vector.x, -vector.y, -vector.z, -vector.w};
}

Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs)
{
    return Vector2{lhs.x + rhs.x, lhs.y + rhs.y};
}

Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs)
{
    return Vector3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

Vector4 operator+(const Vector4 &lhs, const Vector4 &rhs)
{
    return Vector4{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w};
}

Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs)
{
    return Vector2{lhs.x - rhs.x, lhs.y - rhs.y};
}

Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs)
{
    return Vector3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

Vector4 operator-(const Vector4 &lhs, const Vector4 &rhs)
{
    return Vector4{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w};
}

Vector2 operator*(const Vector2 &vector, float scalar)
{
    return Vector2{vector.x * scalar, vector.y * scalar};
}

Vector2 operator*(float scalar, const Vector2 &vector)
{
    return Vector2{vector.x * scalar, vector.y * scalar};
}

Vector3 operator*(const Vector3 &vector, float scalar)
{
    return Vector3{vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

Vector3 operator*(float scalar, const Vector3 &vector)
{
    return Vector3{vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

Vector4 operator*(const Vector4 &vector, float scalar)
{
    return Vector4{vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar};
}

Vector4 operator*(float scalar, const Vector4 &vector)
{
    return Vector4{vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar};
}

Vector2 operator/(const Vector2 &vector, float scalar)
{
    return Vector2{vector.x / scalar, vector.y / scalar};
}

Vector3 operator/(const Vector3 &vector, float scalar)
{
    return Vector3{vector.x / scalar, vector.y / scalar, vector.z / scalar};
}

Vector4 operator/(const Vector4 &vector, float scalar)
{
    return Vector4{vector.x / scalar, vector.y / scalar, vector.z / scalar, vector.w / scalar};
}

Vector2 &operator+=(Vector2 &lhs, const Vector2 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

Vector3 &operator+=(Vector3 &lhs, const Vector3 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

Vector4 &operator+=(Vector4 &lhs, const Vector4 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
    return lhs;
}

Vector2 &operator-=(Vector2 &lhs, const Vector2 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

Vector3 &operator-=(Vector3 &lhs, const Vector3 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

Vector4 &operator-=(Vector4 &lhs, const Vector4 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
    return lhs;
}

Vector2 &operator*=(Vector2 &lhs, float scalar)
{
    lhs.x *= scalar;
    lhs.y *= scalar;
    return lhs;
}

Vector3 &operator*=(Vector3 &lhs, float scalar)
{
    lhs.x *= scalar;
    lhs.y *= scalar;
    lhs.z *= scalar;
    return lhs;
}

Vector4 &operator*=(Vector4 &lhs, float scalar)
{
    lhs.x *= scalar;
    lhs.y *= scalar;
    lhs.z *= scalar;
    lhs.w *= scalar;
    return lhs;
}

float dot(const Vector2 &lhs, const Vector2 &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

float dot(const Vector3 &lhs, const Vector3 &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

float dot(const Vector4 &lhs, const Vector4 &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

Vector3 cross(const Vector3 &lhs, const Vector3 &rhs)
{
    return Vector3{lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x};
}
} // namespace Age::Math
