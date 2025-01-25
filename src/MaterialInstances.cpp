#include "MaterialInstances.hpp"
#include "ShaderInstances.hpp"

namespace Age::Gfx
{
NoLightingMaterial::NoLightingMaterial(const Shader &shader)
    : Material{shader}
{
}

void NoLightingMaterial::apply_properties() const
{
}

NoLightingColorMaterial::NoLightingColorMaterial(const Shader &shader)
    : NoLightingMaterial{shader}
{
}

void NoLightingColorMaterial::apply_properties() const
{
    NoLightingMaterial::apply_properties();
    const auto &shader = static_cast<const NoLightingColorShader &>(this->shader);
    set_uniform(shader.color, color);
}

FragmentLightingMaterial::FragmentLightingMaterial(const Shader &shader)
    : Material{shader}
{
}

void FragmentLightingMaterial::apply_properties() const
{
    const auto &shader = static_cast<const FragmentLightingShader &>(this->shader);
    set_uniform(shader.specular_color, specular_color);
    set_uniform(shader.surface_shininess, surface_shininess);
}

FragmentLightingColorMaterial::FragmentLightingColorMaterial(const Shader &shader)
    : FragmentLightingMaterial{shader}
{
}

void FragmentLightingColorMaterial::apply_properties() const
{
    FragmentLightingMaterial::apply_properties();
    const auto &shader = static_cast<const FragmentLightingColorShader &>(this->shader);
    set_uniform(shader.color, color);
}
} // namespace Age::Gfx
