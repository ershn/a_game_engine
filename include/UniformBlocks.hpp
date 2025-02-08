#pragma once

#include "Matrix.hpp"
#include "UniformBuffer.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
struct ProjectionUniformBlock
{
    Math::Matrix4 view_to_clip_matrix;
};

struct LightsUniformBlock
{
    static constexpr std::size_t LIGHT_COUNT{4};

    struct Light
    {
        Math::Vector4 view_position;
        Math::Vector4 intensity;
    };

    Math::Vector4 ambient_light_intensity;
    float light_attenuation{};
    float _padding_[3];
    Light lights[LIGHT_COUNT];
};

struct FragmentPositionDataUniformBlock
{
    Math::Matrix4 clip_to_view_matrix;
    int width;
    int height;
};

struct MaterialUniformBlock
{
    Math::Vector4 specular_color;
    float surface_shininess;
    float padding[3];
};
} // namespace Age::Gfx
