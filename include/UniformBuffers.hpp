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

class FragmentPositionDataUniformBuffer : public UniformBuffer
{
  public:
    FragmentPositionDataUniformBuffer();

    void set_inverse_clip_matrix(const Math::Matrix4 &matrix);
    void set_viewport_dimensions(int width, int height);
};
} // namespace Age::Gfx
