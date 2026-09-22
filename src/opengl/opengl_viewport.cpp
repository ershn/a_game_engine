#include "opengl/opengl_viewport.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx::OGL
{
namespace
{
Math::RectangleI s_current_viewport_rect{};
float s_current_near_depth{0.0f};
float s_current_far_depth{1.0f};
} // namespace

namespace Api
{
void init_viewport_system()
{
    auto viewport_params = get_integers<4>(GL_VIEWPORT);
    s_current_viewport_rect.size = {viewport_params[2], viewport_params[3]};

    init_viewport_storage();
}

void set_viewport_transformation(const MappedViewport &mapped_viewport)
{
    if (mapped_viewport.rect != s_current_viewport_rect)
    {
        const Math::RectangleI &rect{mapped_viewport.rect};

        glViewport(
            static_cast<GLint>(rect.position.x),
            static_cast<GLint>(rect.position.y),
            static_cast<GLsizei>(rect.size.x),
            static_cast<GLsizei>(rect.size.y)
        );

        s_current_viewport_rect = rect;
    }

    if (mapped_viewport.near_depth != s_current_near_depth || mapped_viewport.far_depth != s_current_far_depth)
    {
        glDepthRange(mapped_viewport.near_depth, mapped_viewport.far_depth);

        s_current_near_depth = mapped_viewport.near_depth;
        s_current_far_depth = mapped_viewport.far_depth;
    }
}
} // namespace Api
} // namespace Age::Gfx::OGL
