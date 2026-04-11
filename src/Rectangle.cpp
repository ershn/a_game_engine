#include "Rectangle.hpp"

namespace Age::Gfx
{
bool operator==(const Rectangle &lhs, const Rectangle &rhs)
{
    return lhs.position == rhs.position && lhs.size == rhs.size;
}

bool operator==(const RectangleI &lhs, const RectangleI &rhs)
{
    return lhs.position == rhs.position && lhs.size == rhs.size;
}

bool operator!=(const Rectangle &lhs, const Rectangle &rhs)
{
    return !(lhs == rhs);
}

bool operator!=(const RectangleI &lhs, const RectangleI &rhs)
{
    return !(lhs == rhs);
}
} // namespace Age::Gfx
