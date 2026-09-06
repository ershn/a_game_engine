#include "opengl/opengl_errors.hpp"
#include "error_handling.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx::OGL
{
void clear_errors()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

void log_error()
{
    switch (glGetError())
    {
    case GL_INVALID_ENUM:
        Core::log_error("OpenGL: INVALID_ENUM error");
        break;
    case GL_INVALID_VALUE:
        Core::log_error("OpenGL: INVALID_VALUE error");
        break;
    case GL_INVALID_OPERATION:
        Core::log_error("OpenGL: INVALID_OPERATION error");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        Core::log_error("OpenGL: INVALID_FRAMEBUFFER_OPERATION error");
        break;
    case GL_OUT_OF_MEMORY:
        Core::log_error("OpenGL: OUT_OF_MEMORY error");
        break;
    }
}
} // namespace Age::Gfx::OGL
