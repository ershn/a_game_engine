#include "opengl/opengl_render_pipeline_state.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx::OGL
{
namespace
{
constexpr GLenum s_comparison_operator_to_gl_enum[] = {
    GL_NEVER, GL_LESS, GL_LEQUAL, GL_EQUAL, GL_GEQUAL, GL_GREATER, GL_NOTEQUAL, GL_ALWAYS
};

constexpr GLenum to_gl_enum(ComparisonOperator comparison)
{
    return s_comparison_operator_to_gl_enum[static_cast<std::size_t>(comparison)];
}

constexpr RenderPipelineState DEFAULT_RENDER_STATE{
    .front_face = FrontFace::COUNTER_CLOCKWISE,
    .cull_mode = CullMode::NONE,
    .depth_clamping = false,
    .depth_testing = false,
    .depth_comparison = ComparisonOperator::LESS
};

RenderPipelineState s_current_render_state{DEFAULT_RENDER_STATE};

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

void enable_depth_clamping(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_CLAMP);
    else
        glDisable(GL_DEPTH_CLAMP);
}

void enable_depth_testing(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void set_depth_comparison(ComparisonOperator depth_comparison)
{
    glDepthFunc(to_gl_enum(depth_comparison));
}
} // namespace

namespace Api
{
void update_render_pipeline_state(RenderPipelineState render_state)
{
    RenderPipelineState current_render_state{s_current_render_state};

    if (render_state.front_face != current_render_state.front_face)
        set_front_face(render_state.front_face);
    if (render_state.cull_mode != current_render_state.cull_mode)
        set_cull_mode(render_state.cull_mode);
    if (render_state.depth_clamping != current_render_state.depth_clamping)
        enable_depth_clamping(render_state.depth_clamping);
    if (render_state.depth_testing != current_render_state.depth_testing)
        enable_depth_testing(render_state.depth_testing);
    if (render_state.depth_comparison != current_render_state.depth_comparison)
        set_depth_comparison(render_state.depth_comparison);

    s_current_render_state = render_state;
}
} // namespace Api
} // namespace Age::Gfx::OGL
