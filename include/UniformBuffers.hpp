#pragma once

#include "Matrix.hpp"
#include "UniformBuffer.hpp"

namespace Age::Gfx
{
class SharedMatricesUniformBuffer : public UniformBuffer
{
  public:
    SharedMatricesUniformBuffer();

    void set_clip_matrix(const Math::Matrix4 &matrix);
};
} // namespace Age::Gfx
