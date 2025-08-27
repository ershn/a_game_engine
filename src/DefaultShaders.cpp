#include "DefaultShaders.hpp"

namespace Age::Gfx
{
UnlitShader::UnlitShader(GLuint shader_program)
    : Shader{shader_program, Gfx::SHADER_LV_MATRIX}
{
}

UnlitColorShader::UnlitColorShader(GLuint shader_program)
    : UnlitShader{shader_program}
    , color{OGL::get_uniform_location(shader_program, "uColor")}
{
}

LitDiffuseTextureShader::LitDiffuseTextureShader(GLuint shader_program)
    : Shader{shader_program, Gfx::SHADER_LV_MATRIX | SHADER_LV_NORMAL_MATRIX | SHADER_LIGHT_DATA_BLOCK}
    , texture_unit{OGL::get_uniform_location(shader_program, "_texture")}
{
}

FragmentLightingShader::FragmentLightingShader(GLuint shader_program)
    : Shader{shader_program, Gfx::SHADER_LV_MATRIX | SHADER_LV_NORMAL_MATRIX | SHADER_LIGHT_DATA_BLOCK}
    , specular_color{OGL::get_uniform_location(shader_program, "uSpecularColor")}
    , surface_shininess{OGL::get_uniform_location(shader_program, "uSurfaceShininess")}
{
}

FragmentLightingColorShader::FragmentLightingColorShader(GLuint shader_program)
    : FragmentLightingShader{shader_program}
    , diffuse_color{OGL::get_uniform_location(shader_program, "uDiffuseColor")}
{
}
} // namespace Age::Gfx
