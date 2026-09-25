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
    .clamp_depth = false,
    .test_depth = false,
    .depth_comparison = ComparisonOperator::LESS,
    .color_masks = {},
    .write_depth = true
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

void set_color_mask(std::size_t index, ColorMask color_mask)
{
    glColorMaski(
        static_cast<GLuint>(index),
        static_cast<GLboolean>(color_mask & ColorMask::R),
        static_cast<GLboolean>(color_mask & ColorMask::G),
        static_cast<GLboolean>(color_mask & ColorMask::B),
        static_cast<GLboolean>(color_mask & ColorMask::A)
    );
}

void enable_depth_writing(bool enable)
{
    glDepthMask(enable);
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

    if (render_state.clamp_depth != current_render_state.clamp_depth)
        enable_depth_clamping(render_state.clamp_depth);
    if (render_state.test_depth != current_render_state.test_depth)
        enable_depth_testing(render_state.test_depth);
    if (render_state.depth_comparison != current_render_state.depth_comparison)
        set_depth_comparison(render_state.depth_comparison);

    for (std::size_t index{}; index < COLOR_ATTACHMENT_POINT_COUNT; ++index)
    {
        if (render_state.color_masks[index] != current_render_state.color_masks[index])
            set_color_mask(index, render_state.color_masks[index]);
    }
    if (render_state.write_depth != current_render_state.write_depth)
        enable_depth_writing(render_state.write_depth);

    s_current_render_state = render_state;
}
} // namespace Api
} // namespace Age::Gfx::OGL
