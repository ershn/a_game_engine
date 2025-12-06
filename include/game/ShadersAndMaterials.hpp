#pragma once

#include "Material.hpp"
#include "Shader.hpp"
#include "UniformBlocks.hpp"
#include "UniformBuffer.hpp"
#include "Vector.hpp"

#include "game/Components.hpp"

namespace Game
{
enum BlockBinding : GLuint
{
    MATERIAL_BLOCK_BINDING = Age::Gfx::USER_BLOCK_BINDING_START_ID,
    MATERIALS_BLOCK_BINDING
};

struct FragmentLightingShader : public Age::Gfx::Shader
{
    GLint gaussian_texture{-1};
    GLuint material_block{GL_INVALID_INDEX};

    FragmentLightingShader(GLuint shader_program);
};

struct FragmentLightingMaterial : public Age::Gfx::Material
{
    int gaussian_texture{};

    FragmentLightingMaterial(Age::Gfx::Shader &shader);

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

    FragmentLightingColorMaterial(Age::Gfx::Shader &shader);

    void apply_properties() const override;
};

struct SphereImpostorShader : public Age::Gfx::Shader
{
    GLuint materials_block{GL_INVALID_INDEX};

    SphereImpostorShader(GLuint shader_program);
};

struct SphereImpostorMaterial : public Age::Gfx::Material
{
    SphereImpostorMaterial(Age::Gfx::Shader &shader);

    void apply_properties() const override;
};
} // namespace Game
