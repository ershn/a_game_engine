#pragma once

#include <concepts>
#include <utility>

namespace Age::Core
{
struct Less
{
    template <typename T, typename U>
    constexpr auto operator()(T &&lhs, U &&rhs) const -> decltype(std::forward<T>(lhs) < std::forward<U>(rhs))
    {
        return std::forward<T>(lhs) < std::forward<U>(rhs);
    }
};

struct LessOrEqual
{
    template <typename T, typename U>
    constexpr auto operator()(T &&lhs, U &&rhs) const -> decltype(std::forward<T>(lhs) <= std::forward<U>(rhs))
    {
        return std::forward<T>(lhs) <= std::forward<U>(rhs);
    }
};

template <typename T>
concept LessComparison = std::same_as<T, Less> || std::same_as<T, LessOrEqual>;
} // namespace Age::Core
