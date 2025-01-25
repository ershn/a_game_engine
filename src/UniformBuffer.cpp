#include "UniformBuffer.hpp"

namespace Age::Gfx
{
GLuint create_uniform_buffer(std::size_t size)
{
    GLuint uniform_buffer_object;

    glGenBuffers(1, &uniform_buffer_object);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_object);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return uniform_buffer_object;
}

void write_uniform_buffer(GLuint uniform_buffer_object, std::size_t offset, const void *data, std::size_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_object);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void bind_uniform_buffer(GLuint uniform_buffer_object, std::size_t size, GLuint binding_point)
{
    glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, uniform_buffer_object, 0, size);
}

UniformBuffer::UniformBuffer(std::size_t size)
    : uniform_buffer_object{create_uniform_buffer(size)}
    , size{size}
{
}

std::vector<std::unique_ptr<UniformBuffer>> g_uniform_buffers{};

void init_uniform_buffer_system()
{
    g_uniform_buffers.reserve(64);
}

const UniformBuffer &get_uniform_buffer(UniformBufferId buffer_id)
{
    return *g_uniform_buffers[buffer_id];
}

void bind_uniform_buffer(const UniformBuffer &buffer, GLuint binding_point)
{
    bind_uniform_buffer(buffer.uniform_buffer_object, buffer.size, binding_point);
}
} // namespace Age::Gfx
