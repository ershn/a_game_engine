#include "vector.hpp"

namespace Age::Math
{
std::ostream &operator<<(std::ostream &out, const Vector2 &vector)
{
    out << std::showpoint << '{' << vector.x << "f, " << vector.y << "f}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Vector2I &vector)
{
    out << '{' << vector.x << ", " << vector.y << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Vector2U &vector)
{
    out << '{' << vector.x << ", " << vector.y << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Vector3 &vector)
{
    out << std::showpoint << '{' << vector.x << "f, " << vector.y << "f, " << vector.z << "f}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Vector3I &vector)
{
    out << '{' << vector.x << ", " << vector.y << ", " << vector.z << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Vector3U &vector)
{
    out << '{' << vector.x << ", " << vector.y << ", " << vector.z << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Vector4 &vector)
{
    out << std::showpoint << '{' << vector.x << "f, " << vector.y << "f, " << vector.z << "f, " << vector.w << "f}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Vector4I &vector)
{
    out << '{' << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << "}";
    return out;
}

std::ostream &operator<<(std::ostream &out, const Vector4U &vector)
{
    out << '{' << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << "}";
    return out;
}

Vector2 pow(const Vector2 &base, const Vector2 &exponent)
{
    return {std::pow(base.x, exponent.x), std::pow(base.y, exponent.y)};
}

Vector3 pow(const Vector3 &base, const Vector3 &exponent)
{
    return {std::pow(base.x, exponent.x), std::pow(base.y, exponent.y), std::pow(base.z, exponent.z)};
}

Vector4 pow(const Vector4 &base, const Vector4 &exponent)
{
    return {
        std::pow(base.x, exponent.x),
        std::pow(base.y, exponent.y),
        std::pow(base.z, exponent.z),
        std::pow(base.w, exponent.w)
    };
}
} // namespace Age::Math
