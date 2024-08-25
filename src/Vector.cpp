#include <sstream>

#include "Vector.hpp"

namespace Age::Math
{
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

float Vector3::length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

Vector3 &Vector3::normalize()
{
    float length{this->length()};
    if (length == 0.0f)
        throw std::domain_error("Cannot normalize a vector of length 0");

    float scale{1.0f / length};
    *this *= scale;
    return *this;
}

Vector3 Vector3::normalized() const
{
    Vector3 result{*this};
    result.normalize();
    return result;
}

Vector3 Vector3::operator-() const
{
    return {-x, -y, -z};
}

Vector3 Vector3::operator*(float scalar) const
{
    Vector3 result{*this};
    result *= scalar;
    return result;
}

Vector3 &Vector3::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3 Vector3::operator+(const Vector3 &rhs) const
{
    Vector3 result{*this};
    result += rhs;
    return result;
}

Vector3 &Vector3::operator+=(const Vector3 &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3 &rhs) const
{
    Vector3 result{*this};
    result -= rhs;
    return result;
}

Vector3 &Vector3::operator-=(const Vector3 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

float Vector3::dot(const Vector3 &rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector3 Vector3::cross(const Vector3 &rhs) const
{
    return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x};
}

std::string Vector3::to_string() const
{
    std::stringstream ss;
    ss << '(' << x << ", " << y << ", " << z << ')';
    return ss.str();
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

float Vector4::length() const
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

Vector4 &Vector4::normalize()
{
    float length{this->length()};
    if (length == 0.0f)
        throw std::domain_error("Cannot normalize a vector of length 0");

    float scale{1.0f / length};
    *this *= scale;
    return *this;
}

Vector4 Vector4::normalized() const
{
    Vector4 result{*this};
    result.normalize();
    return result;
}

Vector4 Vector4::operator-() const
{
    return {-x, -y, -z, -w};
}

Vector4 Vector4::operator*(float scalar) const
{
    Vector4 result{*this};
    result *= scalar;
    return result;
}

Vector4 &Vector4::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vector4 Vector4::operator+(const Vector4 &rhs) const
{
    Vector4 result{*this};
    result += rhs;
    return result;
}

Vector4 &Vector4::operator+=(const Vector4 &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

Vector4 Vector4::operator-(const Vector4 &rhs) const
{
    Vector4 result{*this};
    result -= rhs;
    return result;
}

Vector4 &Vector4::operator-=(const Vector4 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}

float Vector4::dot(const Vector4 &rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

std::string Vector4::to_string() const
{
    std::stringstream ss;
    ss << '(' << x << ", " << y << ", " << z << ", " << w << ')';
    return ss.str();
}
} // namespace Age::Math