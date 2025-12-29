#include "DefaultShaders.hpp"

namespace Age::Gfx
{
UnlitShader::UnlitShader(GLuint shader_program)
    : Shader{shader_program}
{
}

UnlitColorShader::UnlitColorShader(GLuint shader_program)
    : UnlitShader{shader_program}
    , color{OGL::get_uniform_location(shader_program, "_color")}
{
}

LitDiffuseTextureShader::LitDiffuseTextureShader(GLuint shader_program)
    : Shader{shader_program, {.lv_normal_matrix = true}}
    , light_block{OGL::get_uniform_block_index(shader_program, "LightBlock")}
    , sampler{OGL::get_uniform_location(shader_program, "_texture")}
{
}

FragmentLightingShader::FragmentLightingShader(GLuint shader_program)
    : Shader{shader_program, {.lv_normal_matrix = true}}
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
