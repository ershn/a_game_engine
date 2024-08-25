#pragma once

#include <glad/gl.h>

#include <cstddef>

namespace Age::Gfx
{
class UniformBuffer
{
    std::size_t _size{};
    GLuint _uniform_buffer_object{};

  protected:
    UniformBuffer(std::size_t size);
    virtual ~UniformBuffer() = default;

    void copy_data(std::size_t offset, std::size_t size, const void *data);

  public:
    void bind(GLuint binding_point);
};
} // namespace Age::Gfx