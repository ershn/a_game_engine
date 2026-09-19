#include "render_pipeline_state.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx
{
namespace
{
constexpr RenderPipelineState DEFAULT_RENDER_STATE{
    .depth_clamping = false, .front_face = FrontFace::COUNTER_CLOCKWISE, .cull_mode = CullMode::NONE
};

RenderPipelineState s_current_render_state{DEFAULT_RENDER_STATE};

void enable_depth_clamping(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_CLAMP);
    else
        glDisable(GL_DEPTH_CLAMP);
}

void set_front_face(FrontFace front_face)
{
    switch (front_face)
    {
    case FrontFace::COUNTER_CLOCKWISE:
        glFrontFace(GL_CCW);
        break;
    case FrontFace::CLOCKWISE:
        glFrontFace(GL_CW);
        break;
    }
}

void set_cull_mode(CullMode cull_mode)
{
    switch (cull_mode)
    {
    case CullMode::NONE:
        glDisable(GL_CULL_FACE);
        break;
    case CullMode::FRONT:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case CullMode::BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    case CullMode::FRONT_AND_BACK:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT_AND_BACK);
        break;
    }
}
} // namespace

void update_render_pipeline_state(RenderPipelineState render_state)
{
    RenderPipelineState current_render_state{s_current_render_state};

    if (render_state.depth_clamping != current_render_state.depth_clamping)
        enable_depth_clamping(render_state.depth_clamping);
    if (render_state.front_face != current_render_state.front_face)
        set_front_face(render_state.front_face);
    if (render_state.cull_mode != current_render_state.cull_mode)
        set_cull_mode(render_state.cull_mode);

    s_current_render_state = render_state;
}
} // namespace Age::Gfx
