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
} // namespace Age::Gfx