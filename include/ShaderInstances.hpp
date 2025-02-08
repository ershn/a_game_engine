#pragma once

#include "Shader.hpp"

namespace Age::Gfx
{
struct NoLightingShader : public Shader
{
    NoLightingShader(GLuint shader_program);
};

struct NoLightingColorShader : public NoLightingShader
{
    GLint color{-1};

    NoLightingColorShader(GLuint shader_program);
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
