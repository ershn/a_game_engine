#include "DefaultMaterials.hpp"
#include "DefaultShaders.hpp"

namespace Age::Gfx
{
UnlitMaterial::UnlitMaterial(Shader &shader)
    : Material{shader}
{
}

void UnlitMaterial::apply_properties() const
{
}

UnlitColorMaterial::UnlitColorMaterial(Shader &shader)
    : UnlitMaterial{shader}
{
}

void UnlitColorMaterial::apply_properties() const
{
    UnlitMaterial::apply_properties();
    const auto &shader = static_cast<const UnlitColorShader &>(this->shader);
    OGL::set_uniform(shader.color, color);
}

LitDiffuseTextureMaterial::LitDiffuseTextureMaterial(Shader &shader)
    : Material{shader}
{
}

void LitDiffuseTextureMaterial::apply_properties() const
{
    auto &shader = static_cast<LitDiffuseTextureShader &>(this->shader);
    bind_texture_and_sampler(shader.sampler, texture_id, sampler_id);
}

FragmentLightingMaterial::FragmentLightingMaterial(Shader &shader)
    : Material{shader}
{
}

void FragmentLightingMaterial::apply_properties() const
{
    const auto &shader = static_cast<const FragmentLightingShader &>(this->shader);
    OGL::set_uniform(shader.specular_color, specular_color);
    OGL::set_uniform(shader.surface_shininess, surface_shininess);
}

FragmentLightingColorMaterial::FragmentLightingColorMaterial(Shader &shader)
    : FragmentLightingMaterial{shader}
{
}

void FragmentLightingColorMaterial::apply_properties() const
{
    FragmentLightingMaterial::apply_properties();
    const auto &shader = static_cast<const FragmentLightingColorShader &>(this->shader);
    OGL::set_uniform(shader.diffuse_color, diffuse_color);
}
} // namespace Age::Gfx
