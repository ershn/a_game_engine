#pragma once

#include <cstdint>

#include "common_framebuffer.hpp"
#include "utils/default.hpp"
#include "utils/types.hpp"

namespace Age::Gfx
{
enum struct Enable : std::uint8_t
{
    OFF,
    ON
};

enum struct ComparisonOperator : std::uint8_t
{
    NEVER,
    LESS,
    LESS_OR_EQUAL,
    EQUAL,
    GREATER_OR_EQUAL,
    GREATER,
    NOT_EQUAL,
    ALWAYS
};

enum struct FrontFace : std::uint8_t
{
    COUNTER_CLOCKWISE,
    CLOCKWISE
};

enum struct CullMode : std::uint8_t
{
    NONE,
    FRONT,
    BACK,
    FRONT_AND_BACK
};

enum struct ColorMask : std::uint8_t
{
    R = 0b0001,
    G = 0b0010,
    B = 0b0100,
    A = 0b1000,
    RGB = R | G | B,
    RGBA = R | G | B | A,
};

constexpr ColorMask operator~(ColorMask mask)
{
    return static_cast<ColorMask>(~Utils::to_underlying(mask));
}

constexpr ColorMask operator&(ColorMask lhs, ColorMask rhs)
{
    return static_cast<ColorMask>(Utils::to_underlying(lhs) & Utils::to_underlying(rhs));
}

constexpr ColorMask operator|(ColorMask lhs, ColorMask rhs)
{
    return static_cast<ColorMask>(Utils::to_underlying(lhs) | Utils::to_underlying(rhs));
}

constexpr ColorMask operator^(ColorMask lhs, ColorMask rhs)
{
    return static_cast<ColorMask>(Utils::to_underlying(lhs) ^ Utils::to_underlying(rhs));
}

struct RenderPipelineState
{
    struct Diff
    {
        FrontFace front_face : 2 {0b11};
        CullMode cull_mode : 3 {0b111};
        Enable clamp_depth : 2 {0b11};
        Enable test_depth : 2 {0b11};
        ComparisonOperator depth_comparison : 4 {0b1111};
        std::array<Utils::Default<ColorMask{0xFF}>, COLOR_ATTACHMENT_POINT_COUNT> color_masks{};
        Enable write_depth : 2 {0b11};
    };

    FrontFace front_face : 1 {FrontFace::CLOCKWISE};
    CullMode cull_mode : 2 {CullMode::BACK};
    bool clamp_depth : 1 {false};
    bool test_depth : 1 {true};
    ComparisonOperator depth_comparison : 3 {ComparisonOperator::LESS_OR_EQUAL};
    std::array<Utils::Default<ColorMask::RGBA>, COLOR_ATTACHMENT_POINT_COUNT> color_masks{};
    bool write_depth : 1 {true};

    constexpr void apply(Diff diff)
    {
        constexpr Diff defaults; // Using Diff{} directly causes a compilation error on msvc

        if (diff.front_face != defaults.front_face)
            front_face = diff.front_face;
        if (diff.cull_mode != defaults.cull_mode)
            cull_mode = diff.cull_mode;

        if (diff.clamp_depth != defaults.clamp_depth)
            clamp_depth = static_cast<bool>(diff.clamp_depth);
        if (diff.test_depth != defaults.test_depth)
            test_depth = static_cast<bool>(diff.test_depth);
        if (diff.depth_comparison != defaults.depth_comparison)
            depth_comparison = diff.depth_comparison;

        for (std::size_t index{}; index < COLOR_ATTACHMENT_POINT_COUNT; ++index)
        {
            if (diff.color_masks[index] != ColorMask{0xFF})
                color_masks[index] = diff.color_masks[index];
        }
        if (diff.write_depth != defaults.write_depth)
            write_depth = static_cast<bool>(diff.write_depth);
    }
};
} // namespace Age::Gfx
