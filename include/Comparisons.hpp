#pragma once

#include <type_traits>
#include <utility>

namespace Age::Util
{
struct Less
{
    template <typename T, typename U>
    constexpr auto operator()(T &&lhs, U &&rhs) const -> decltype(std::forward<T>(lhs) < std::forward<U>(rhs))
    {
        return std::forward<T>(lhs) < std::forward<U>(rhs);
    }
};

struct LessEqual
{
    template <typename T, typename U>
    constexpr auto operator()(T &&lhs, U &&rhs) const -> decltype(std::forward<T>(lhs) <= std::forward<U>(rhs))
    {
        return std::forward<T>(lhs) <= std::forward<U>(rhs);
    }
};

template <typename T>
concept LessComparison = std::is_same_v<T, Less> || std::is_same_v<T, LessEqual>;
} // namespace Age::Util
