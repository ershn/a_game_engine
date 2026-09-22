#pragma once

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
        : position{rect.position}
        , size{rect.size}
    {
    }

    constexpr bool operator==(const RectangleX<T> &rhs) const = default;
};

using Rectangle = RectangleX<float>;
using RectangleI = RectangleX<int>;
using RectangleU = RectangleX<unsigned int>;

template <typename T>
constexpr RectangleX<T> scale(const RectangleX<T> &rect, const Vector2X<T> &scaling)
{
    return {Math::scale(rect.position, scaling), Math::scale(rect.size, scaling)};
}

Rectangle round(const Rectangle &rect);
} // namespace Age::Math
