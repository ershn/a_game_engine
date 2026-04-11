#pragma once

#include "Vector.hpp"

namespace Age::Gfx
{
struct Rectangle
{
    Math::Vector2 position{};
    Math::Vector2 size{};
};

struct RectangleI
{
    Math::Vector2I position{};
    Math::Vector2I size{};
};

bool operator==(const Rectangle &lhs, const Rectangle &rhs);
bool operator==(const RectangleI &lhs, const RectangleI &rhs);

bool operator!=(const Rectangle &lhs, const Rectangle &rhs);
bool operator!=(const RectangleI &lhs, const RectangleI &rhs);
} // namespace Age::Gfx
