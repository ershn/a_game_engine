#include "UniformBuffers.hpp"

namespace Age::Gfx
{
SharedMatricesUniformBuffer::SharedMatricesUniformBuffer()
    : UniformBuffer{sizeof(Math::Matrix4)}
{
}

void SharedMatricesUniformBuffer::set_clip_matrix(const Math::Matrix4 &matrix)
{
    copy_data(0, sizeof(Math::Matrix4), static_cast<const float *>(matrix));
}

FragmentPositionDataUniformBuffer::FragmentPositionDataUniformBuffer()
    : UniformBuffer{sizeof(Math::Matrix4) + sizeof(int) * 2}
{
}

void FragmentPositionDataUniformBuffer::set_inverse_clip_matrix(const Math::Matrix4 &matrix)
{
    copy_data(0, sizeof(Math::Matrix4), static_cast<const float *>(matrix));
}

void FragmentPositionDataUniformBuffer::set_viewport_dimensions(int width, int height)
{
    int dimensions[]{width, height};

    copy_data(sizeof(Math::Matrix4), sizeof(dimensions), &dimensions);
}
} // namespace Age::Gfx
