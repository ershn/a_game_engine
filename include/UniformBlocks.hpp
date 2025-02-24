#pragma once

#include "Matrix.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
struct GammaCorrectionBlock
{
    float gamma_inverse{};
    float _padding_[3];
};

struct ProjectionBlock
{
    Math::Matrix4 view_to_clip_matrix;
};

struct LightsBlock
{
    static constexpr std::size_t LIGHT_COUNT{4};

    struct Light
    {
        Math::Vector4 view_position;
        Math::Vector4 intensity;
    };

    Math::Vector4 ambient_light_intensity;
    float light_attenuation{};
    float max_intensity{};
    float _padding_[2];
    Light lights[LIGHT_COUNT];
};

struct FragmentPositionDataBlock
{
    Math::Matrix4 clip_to_view_matrix;
    int width{};
    int height{};
};

struct MaterialBlock
{
    Math::Vector4 specular_color;
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

    Material materials[Count];
};
} // namespace Age::Gfx
