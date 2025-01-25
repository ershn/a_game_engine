#pragma once

#include <numbers>

namespace Age::Math
{
template <typename T>
constexpr T radians(T degrees)
{
    return degrees * std::numbers::pi_v<T> / static_cast<T>(180.0);
}
} // namespace Age::Math
