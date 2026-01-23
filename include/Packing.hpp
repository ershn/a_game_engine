#pragma once

#include <bit>
#include <cstdint>

namespace Age::Packing
{
constexpr std::int16_t normalized_int16(float n)
{
    return static_cast<std::int16_t>(n * 0x7fff);
}

constexpr std::uint16_t normalized_uint16(float n)
{
    return static_cast<std::uint16_t>(n * 0xffff);
}

constexpr std::uint32_t in_uint32(std::int16_t a, std::int16_t b)
{
    return static_cast<std::uint32_t>(std::bit_cast<std::uint16_t>(a)) |
           static_cast<std::uint32_t>(std::bit_cast<std::uint16_t>(b)) << 16;
}

constexpr std::uint32_t in_uint32(std::uint16_t a, std::uint16_t b)
{
    return static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b) << 16;
}

template <unsigned int B>
constexpr std::uint32_t in_uint(int n)
{
    return n & ((1 << (B - 1)) - 1) | static_cast<unsigned int>(n < 0) << (B - 1);
}

constexpr std::uint32_t packed_int(int x, int y, int z, int w)
{
    return in_uint<10u>(x) | in_uint<10u>(y) << 10 | in_uint<10u>(z) << 20 | in_uint<2u>(w) << 30;
}

constexpr std::uint32_t packed_int(float x, float y, float z, float w)
{
    return packed_int(
        static_cast<int>(x * 0x1ff), static_cast<int>(y * 0x1ff), static_cast<int>(z * 0x1ff), static_cast<int>(w)
    );
}

constexpr std::uint32_t packed_uint(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
{
    return x & 0x3ff | (y & 0x3ff) << 10 | (z & 0x3ff) << 20 | (w & 0x3) << 30;
}

constexpr std::uint32_t packed_uint(float x, float y, float z, float w)
{
    return packed_uint(
        static_cast<unsigned int>(x * 0x3ff),
        static_cast<unsigned int>(y * 0x3ff),
        static_cast<unsigned int>(z * 0x3ff),
        static_cast<unsigned int>(w * 0x3)
    );
}
} // namespace Age::Packing
