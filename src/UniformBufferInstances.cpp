#include <cstddef>

#include "UniformBufferInstances.hpp"

namespace Age::Gfx
{
ProjectionUniformBuffer::ProjectionUniformBuffer()
    : UniformBuffer{sizeof(Math::Matrix4)}
{
}

void ProjectionUniformBuffer::set_camera_to_clip_matrix(const Math::Matrix4 &matrix) const
{
    write_uniform_buffer(uniform_buffer_object, 0, &matrix, sizeof(matrix));
}

LightDataUniformBuffer::LightDataUniformBuffer()
    : UniformBuffer{sizeof(Buffer)}
{
}

void LightDataUniformBuffer::set_camera_light_position(const Math::Vector3 &camera_light_position) const
{
    write_uniform_buffer(uniform_buffer_object, offsetof(Buffer, camera_light_position), &camera_light_position,
                         sizeof(camera_light_position));
}

void LightDataUniformBuffer::set_light_intensity(const Math::Vector4 &light_intensity) const
{
    write_uniform_buffer(uniform_buffer_object, offsetof(Buffer, light_intensity), &light_intensity,
                         sizeof(light_intensity));
}

void LightDataUniformBuffer::set_light_attenuation(float light_attenuation) const
{
    write_uniform_buffer(uniform_buffer_object, offsetof(Buffer, light_attenuation), &light_attenuation,
                         sizeof(light_attenuation));
}

void LightDataUniformBuffer::set_ambient_light_intensity(const Math::Vector4 &ambient_light_intensity) const
{
    write_uniform_buffer(uniform_buffer_object, offsetof(Buffer, ambient_light_intensity), &ambient_light_intensity,
                         sizeof(ambient_light_intensity));
}

FragmentPositionDataUniformBuffer::FragmentPositionDataUniformBuffer()
    : UniformBuffer{sizeof(Buffer)}
{
}

void FragmentPositionDataUniformBuffer::set_inverse_clip_matrix(const Math::Matrix4 &clip_to_camera_matrix) const
{
    write_uniform_buffer(uniform_buffer_object, offsetof(Buffer, clip_to_camera_matrix), &clip_to_camera_matrix,
                         sizeof(clip_to_camera_matrix));
}

void FragmentPositionDataUniformBuffer::set_viewport_dimensions(int width, int height) const
{
    int dimensions[] = {width, height};

    write_uniform_buffer(uniform_buffer_object, offsetof(Buffer, width), &dimensions, sizeof(dimensions));
}
} // namespace Age::Gfx
