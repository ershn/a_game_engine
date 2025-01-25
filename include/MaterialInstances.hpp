#pragma once

#include "Material.hpp"

namespace Age::Gfx
{
struct NoLightingMaterial : public Material
{
    NoLightingMaterial(const Shader &shader);

    void apply_properties() const override;
};

struct NoLightingColorMaterial : public NoLightingMaterial
{
    Math::Vector4 color{1.0f};

    NoLightingColorMaterial(const Shader &shader);

    void apply_properties() const override;
};

struct FragmentLightingMaterial : public Material
{
    Math::Vector4 specular_color{1.0f};
    float surface_shininess{0.5f};

    FragmentLightingMaterial(const Shader &shader);

    void apply_properties() const override;
};

struct FragmentLightingColorMaterial : public FragmentLightingMaterial
{
    Math::Vector4 color{1.0f};

    FragmentLightingColorMaterial(const Shader &shader);

    void apply_properties() const override;
};
} // namespace Age::Gfx
