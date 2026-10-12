#pragma once

#include <cstddef>

namespace Age::Memory
{
template <std::size_t N>
constexpr std::size_t padding_to(std::size_t size)
{
    return N - size % N;
}
} // namespace Age::Memory
