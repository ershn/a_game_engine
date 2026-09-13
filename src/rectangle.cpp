#include "rectangle.hpp"

namespace Age::Math
{
Rectangle round(const Rectangle &rect)
{
    return {Math::round(rect.position), Math::round(rect.size)};
}
} // namespace Age::Math
