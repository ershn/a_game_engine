#pragma once

namespace Age::Gfx
{
template <bool WithDefaults>
struct RenderPipelineStateImpl
{
    bool depth_clamping : 1 {WithDefaults ? false : false};
    bool srgb_rendering : 1 {WithDefaults ? true : false};

    static constexpr RenderPipelineStateImpl<WithDefaults> from(unsigned int render_state)
    {
        return {
            .depth_clamping = static_cast<bool>(render_state >> 1 & 0b1),
            .srgb_rendering = static_cast<bool>(render_state & 0b1)
        };
    }

    constexpr operator unsigned int() const
    {
        return depth_clamping << 1 | static_cast<unsigned int>(srgb_rendering);
    }
};

using RenderPipelineState = RenderPipelineStateImpl<true>;
using RenderPipelineStateMask = RenderPipelineStateImpl<false>;

void update_render_pipeline_state(RenderPipelineState render_state);
} // namespace Age::Gfx
