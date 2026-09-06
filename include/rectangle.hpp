#pragma once

#include <cmath>

#include "vector.hpp"

namespace Age::Math
{
template <typename T>
struct RectangleX
{
    Vector2X<T> position;
    Vector2X<T> size;

    constexpr RectangleX() = default;

    constexpr RectangleX(const Vector2X<T> &position, const Vector2X<T> &size)
        : position{position}
        , size{size}
    {
    }

    template <typename U>
    explicit constexpr RectangleX(const RectangleX<U> &rect)
        : position{static_cast<Vector2X<T>>(rect.position)}
        , size{static_cast<Vector2X<T>>(rect.size)}
    {
    }

    constexpr bool operator==(const RectangleX<T> &rhs) const = default;
};

using Rectangle = RectangleX<float>;
using RectangleI = RectangleX<int>;
using RectangleU = RectangleX<unsigned int>;

template <typename T>
constexpr Rectangle scale(const Rectangle &rect, const Vector2X<T> &scaling)
{
    return {
        {std::round(rect.position.x * scaling.x), std::round(rect.position.y * scaling.y)},
        {std::round(rect.size.x * scaling.x), std::round(rect.size.y * scaling.y)}
    };
}
} // namespace Age::Math
