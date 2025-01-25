#pragma once

#include "Matrix.hpp"
#include "UniformBuffer.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
struct ProjectionUniformBuffer : public UniformBuffer
{
    ProjectionUniformBuffer();

    void set_camera_to_clip_matrix(const Math::Matrix4 &matrix) const;
};

struct LightDataUniformBuffer : public UniformBuffer
{
    struct Buffer
    {
        Math::Vector4 light_intensity;
        Math::Vector4 ambient_light_intensity;
        Math::Vector3 camera_light_position;
        float light_attenuation;
    };

    LightDataUniformBuffer();

    void set_camera_light_position(const Math::Vector3 &camera_light_position) const;
    void set_light_intensity(const Math::Vector4 &light_intensity) const;
    void set_light_attenuation(float light_attenuation) const;
    void set_ambient_light_intensity(const Math::Vector4 &ambient_light_intensity) const;
};

struct FragmentPositionDataUniformBuffer : public UniformBuffer
{
    struct Buffer
    {
        Math::Matrix4 clip_to_camera_matrix;
        int width;
        int height;
    };

    FragmentPositionDataUniformBuffer();

    void set_inverse_clip_matrix(const Math::Matrix4 &clip_to_camera_matrix) const;
    void set_viewport_dimensions(int width, int height) const;
};
} // namespace Age::Gfx
