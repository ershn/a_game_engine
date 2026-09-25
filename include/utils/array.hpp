#pragma once

#include <array>

namespace Age::Utils
{
template <std::size_t N, typename T>
consteval std::array<T, N> filled_array(const T &value)
{
    std::array<T, N> array;
    array.fill(value);
    return array;
}
} // namespace Age::Utils
