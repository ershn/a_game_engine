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

struct FragmentLightingMaterial : public Age::Gfx::Material
{
    FragmentLightingMaterial(const Age::Gfx::Shader &shader);

    void apply_properties() const override;
};

struct FragmentLightingColorShader : public FragmentLightingShader
{
    GLint diffuse_color{-1};

    FragmentLightingColorShader(GLuint shader_program);
};

struct FragmentLightingColorMaterial : public FragmentLightingMaterial
{
    Age::Math::Vector4 diffuse_color{1.0f};

    FragmentLightingColorMaterial(const Age::Gfx::Shader &shader);

    void apply_properties() const override;
};

struct SphereImpostorShader : public Age::Gfx::Shader
{
    GLuint material_block{GL_INVALID_INDEX};
    GLint sphere_view_position{-1};
    GLint sphere_radius{-1};
    GLint diffuse_color{-1};

    SphereImpostorShader(GLuint shader_program);
};

struct SphereImpostorMaterial : public Age::Gfx::Material
{
    Age::Math::Vector3 sphere_view_position{};
    float sphere_radius{};
    Age::Math::Vector4 diffuse_color{1.0f};

    SphereImpostorMaterial(const Age::Gfx::Shader &shader);

    void apply_properties() const override;
};
} // namespace Game
