#include "UniformBuffer.hpp"

namespace Age::Gfx
{
UniformBuffer::UniformBuffer(std::size_t size)
    : _size{size}
{
    glGenBuffers(1, &_uniform_buffer_object);
    glBindBuffer(GL_UNIFORM_BUFFER, _uniform_buffer_object);
    glBufferData(GL_UNIFORM_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::copy_data(std::size_t offset, std::size_t size, const void *data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, _uniform_buffer_object);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::bind(GLuint binding_point)
{
    glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, _uniform_buffer_object, 0, _size);
}
} // namespace Age::Gfx