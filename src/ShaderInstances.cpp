#include "ShaderInstances.hpp"

namespace Age::Gfx
{
NoLightingShader::NoLightingShader(GLuint shader_program)
    : Shader{shader_program}
{
}

NoLightingColorShader::NoLightingColorShader(GLuint shader_program)
    : NoLightingShader{shader_program}
    , color{get_uniform_location(shader_program, "uColor")}
{
}

FragmentLightingShader::FragmentLightingShader(GLuint shader_program)
    : Shader{shader_program, SHADER_CAMERA_NORMAL_MATRIX | SHADER_LIGHT_DATA_BLOCK}
    , specular_color{get_uniform_location(shader_program, "uSpecularColor")}
    , surface_shininess{get_uniform_location(shader_program, "uSurfaceShininess")}
{
}

FragmentLightingColorShader::FragmentLightingColorShader(GLuint shader_program)
    : FragmentLightingShader{shader_program}
    , color{get_uniform_location(shader_program, "uColor")}
{
}
} // namespace Age::Gfx
