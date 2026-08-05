#include "render_pipeline_state.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx
{
namespace
{
constexpr RenderPipelineState DEFAULT_RENDER_PIPELINE_STATE{.depth_clamping = false, .srgb_rendering = false};

RenderPipelineState s_current_render_pipeline_state{DEFAULT_RENDER_PIPELINE_STATE};
} // namespace

void update_render_pipeline_state(RenderPipelineState render_state)
{
    RenderPipelineState current_render_state{s_current_render_pipeline_state};

    if (render_state.depth_clamping != current_render_state.depth_clamping)
        OGL::enable_depth_clamping(render_state.depth_clamping);
    if (render_state.srgb_rendering != current_render_state.srgb_rendering)
        OGL::enable_srgb_rendering(render_state.srgb_rendering);

    s_current_render_pipeline_state = render_state;
}
} // namespace Age::Gfx
