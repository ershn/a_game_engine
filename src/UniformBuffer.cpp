#include "UniformBuffer.hpp"

namespace Age::Gfx
{
namespace
{
GLint s_uniform_buffer_alignment;
} // namespace

std::size_t get_uniform_buffer_alignment()
{
    return static_cast<std::size_t>(s_uniform_buffer_alignment);
}

std::size_t get_uniform_block_alignment(std::size_t block_size)
{
    return block_size + s_uniform_buffer_alignment - (block_size % s_uniform_buffer_alignment);
}

UniformBuffer::UniformBuffer(std::size_t size)
    : uniform_buffer_object{OGL::create_uniform_buffer(size)}
    , size{size}
{
}

std::vector<std::unique_ptr<UniformBuffer>> g_uniform_buffers{};

void init_uniform_buffer_system()
{
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &s_uniform_buffer_alignment);

    g_uniform_buffers.reserve(64);
}

const UniformBuffer &get_uniform_buffer(UniformBufferId buffer_id)
{
    return *g_uniform_buffers[buffer_id];
}

void bind_uniform_buffer(GLuint binding_point, const UniformBufferRange &buffer_range)
{
    OGL::bind_uniform_buffer_range(binding_point, buffer_range.uniform_buffer_object, buffer_range.offset,
                                   buffer_range.size);
}

void bind_uniform_buffer(const UniformBufferRangeBind &buffer_bind)
{
    OGL::bind_uniform_buffer_range(buffer_bind.binding_point, buffer_bind.buffer_range.uniform_buffer_object,
                                   buffer_bind.buffer_range.offset, buffer_bind.buffer_range.size);
}
} // namespace Age::Gfx
