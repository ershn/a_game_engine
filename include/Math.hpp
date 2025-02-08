#pragma once

#include <numbers>

namespace Age::Math
{
inline constexpr float PI{std::numbers::pi_v<float>};
inline constexpr float TAU{2.0f * std::numbers::pi_v<float>};

template <typename T>
constexpr T radians(T degrees)
{
    return degrees * std::numbers::pi_v<T> / static_cast<T>(180.0);
}

template <typename T>
auto lerp(T a, T b, float t)
{
    return a + (b - a) * t;
}

template <typename T>
float inverse_lerp(T a, T b, T i)
{
    return (i - a) / (b - a);
}
} // namespace Age::Math
