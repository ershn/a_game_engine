#include "render_pipeline_state.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx
{
namespace
{
constexpr RenderPipelineState DEFAULT_RENDER_STATE{.depth_clamping = false};

RenderPipelineState s_current_render_state{DEFAULT_RENDER_STATE};
} // namespace

void update_render_pipeline_state(RenderPipelineState render_state)
{
    RenderPipelineState current_render_state{s_current_render_state};

    if (render_state.depth_clamping != current_render_state.depth_clamping)
        OGL::enable_depth_clamping(render_state.depth_clamping);

    s_current_render_state = render_state;
}
} // namespace Age::Gfx
