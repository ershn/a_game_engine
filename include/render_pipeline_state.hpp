#pragma once

#include <cstdint>

namespace Age::Gfx
{
enum struct DepthClamping : std::uint8_t
{
    OFF,
    ON
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
        DepthClamping depth_clamping : 2 {0b11};
        FrontFace front_face : 2 {0b11};
        CullMode cull_mode : 3 {0b111};
    };

    bool depth_clamping : 1 {false};
    FrontFace front_face : 1 {FrontFace::CLOCKWISE};
    CullMode cull_mode : 2 {CullMode::BACK};

    constexpr void apply(Diff diff)
    {
        if (diff.depth_clamping != Diff{}.depth_clamping)
            depth_clamping = static_cast<bool>(diff.depth_clamping);
        if (diff.front_face != Diff{}.front_face)
            front_face = diff.front_face;
        if (diff.cull_mode != Diff{}.cull_mode)
            cull_mode = diff.cull_mode;
    }
};

void update_render_pipeline_state(RenderPipelineState render_state);
} // namespace Age::Gfx
