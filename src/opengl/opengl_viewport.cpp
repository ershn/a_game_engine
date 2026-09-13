#include "opengl/opengl_viewport.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx::OGL
{
namespace
{
Math::RectangleI s_current_viewport_rect{};
} // namespace

namespace Api
{
void use_viewport_rect(const Math::RectangleI &rect)
{
    if (rect != s_current_viewport_rect)
    {
        glViewport(
            static_cast<GLint>(rect.position.x),
            static_cast<GLint>(rect.position.y),
            static_cast<GLsizei>(rect.size.x),
            static_cast<GLsizei>(rect.size.y)
        );

        s_current_viewport_rect = rect;
    }
}
} // namespace Api
} // namespace Age::Gfx::OGL
