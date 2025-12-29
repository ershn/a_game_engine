#pragma once

#include "Material.hpp"
#include "Shader.hpp"
#include "UniformBlocks.hpp"
#include "UniformBuffer.hpp"
#include "Vector.hpp"

#include "game/Components.hpp"

namespace Game
{
struct FragmentLightingShader : public Age::Gfx::Shader
{
    Age::Gfx::UniformBlock light_block{};
    Age::Gfx::UniformBlock material_block{};
    GLint gaussian_texture{-1};

    FragmentLightingShader(GLuint shader_program);
};

struct FragmentLightingMaterial : public Age::Gfx::Material
{
    Age::Gfx::UniformBufferRangeId light_buffer_range_id{};
    Age::Gfx::UniformBufferRangeId material_buffer_range_id{};
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
} // namespace Game
