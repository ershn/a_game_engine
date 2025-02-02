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

struct LightDataUniformBlock
{
    Math::Vector4 light_intensity;
    Math::Vector4 ambient_light_intensity;
    Math::Vector3 view_light_position;
    float light_attenuation;
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
