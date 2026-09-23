#pragma once

#include <cstdint>

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

struct RenderPipelineState
{
    struct Diff
    {
        FrontFace front_face : 2 {0b11};
        CullMode cull_mode : 3 {0b111};
        Enable depth_clamping : 2 {0b11};
        Enable depth_testing : 2 {0b11};
        ComparisonOperator depth_comparison : 4 {0b1111};
    };

    FrontFace front_face : 1 {FrontFace::CLOCKWISE};
    CullMode cull_mode : 2 {CullMode::BACK};
    bool depth_clamping : 1 {false};
    bool depth_testing : 1 {true};
    ComparisonOperator depth_comparison : 3 {ComparisonOperator::LESS_OR_EQUAL};

    constexpr void apply(Diff diff)
    {
        if (diff.front_face != Diff{}.front_face)
            front_face = diff.front_face;
        if (diff.cull_mode != Diff{}.cull_mode)
            cull_mode = diff.cull_mode;
        if (diff.depth_clamping != Diff{}.depth_clamping)
            depth_clamping = static_cast<bool>(diff.depth_clamping);
        if (diff.depth_testing != Diff{}.depth_testing)
            depth_testing = static_cast<bool>(diff.depth_testing);
        if (diff.depth_comparison != Diff{}.depth_comparison)
            depth_comparison = diff.depth_comparison;
    }
};

void update_render_pipeline_state(RenderPipelineState render_state);
} // namespace Age::Gfx
