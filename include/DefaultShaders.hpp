#pragma once

#include "Shader.hpp"
#include "Texture.hpp"

namespace Age::Gfx
{
struct UnlitShader : public Shader
{
    UnlitShader(GLuint shader_program);
};

struct UnlitColorShader : public UnlitShader
{
    GLint color{-1};

    UnlitColorShader(GLuint shader_program);
};

struct LitDiffuseTextureShader : public Shader
{
    SamplerUniform sampler{};

    LitDiffuseTextureShader(GLuint shader_program);
};

struct FragmentLightingShader : public Shader
{
    GLint specular_color{-1};
    GLint surface_shininess{-1};

    FragmentLightingShader(GLuint shader_program);
};

struct FragmentLightingColorShader : public FragmentLightingShader
{
    GLint diffuse_color{-1};

    FragmentLightingColorShader(GLuint shader_program);
};
} // namespace Age::Gfx
