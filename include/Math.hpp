#pragma once

#include <numbers>

namespace Age::Math
{
template <typename T> constexpr T radians(T degrees)
{
    return degrees * static_cast<T>(2.0) * std::numbers::pi_v<T> / static_cast<T>(360.0);
}
} // namespace Age::Math