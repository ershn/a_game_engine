#pragma once

#include "Material.hpp"
#include "Shader.hpp"
#include "Vector.hpp"

namespace Game
{
inline constexpr GLuint MATERIAL_BLOCK_BINDING{Age::Gfx::USER_BLOCK_BINDING_START_ID};

struct FragmentLightingShader : public Age::Gfx::Shader
{
    GLuint material_block{GL_INVALID_INDEX};

    FragmentLightingShader(GLuint shader_program);
};

struct FragmentLightingColorShader : public FragmentLightingShader
{
    GLint diffuse_color{-1};

    FragmentLightingColorShader(GLuint shader_program);
};

////////////////////////////////////////

struct FragmentLightingMaterial : public Age::Gfx::Material
{
    FragmentLightingMaterial(const Age::Gfx::Shader &shader);

    void apply_properties() const override;
};

struct FragmentLightingColorMaterial : public FragmentLightingMaterial
{
    Age::Math::Vector4 diffuse_color{1.0f};

    FragmentLightingColorMaterial(const Age::Gfx::Shader &shader);

    void apply_properties() const override;
};
} // namespace Game
