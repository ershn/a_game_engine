#pragma once

#include "Matrix.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
struct FragmentPositionDataBlock
{
    Math::Matrix4 clip_to_view_matrix{};
    int width{};
    int height{};
};

struct MaterialBlock
{
    Math::Vector4 specular_color{};
    float surface_shininess{1.0f};
    float _padding_[3];
};

template <std::size_t Count>
struct MaterialsBlock
{
    struct Material
    {
        Age::Math::Vector4 diffuse_color{1.0f};
        Age::Math::Vector4 specular_color{1.0f};
        float surface_shininess{1.0f};
        float _padding_[3];
    };

    Material materials[Count]{};
};
} // namespace Age::Gfx
