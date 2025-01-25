#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "OpenGL.hpp"

namespace Age::Gfx
{
using UniformBufferId = std::uint16_t;

GLuint create_uniform_buffer(std::size_t size);
void write_uniform_buffer(GLuint uniform_buffer_object, std::size_t offset, const void *data, std::size_t size);
void bind_uniform_buffer(GLuint uniform_buffer_object, std::size_t size, GLuint binding_point);

struct UniformBuffer
{
    GLuint uniform_buffer_object{};
    std::size_t size{};

    UniformBuffer(std::size_t size);
};

extern std::vector<std::unique_ptr<UniformBuffer>> g_uniform_buffers;

void init_uniform_buffer_system();

template <typename TUniformBuffer>
void create_uniform_buffer(UniformBufferId buffer_id, GLuint binding_point)
{
    if (buffer_id >= g_uniform_buffers.size())
        g_uniform_buffers.resize(buffer_id + 1);

    g_uniform_buffers[buffer_id] = std::make_unique<TUniformBuffer>();

    const UniformBuffer &uniform_buffer{*g_uniform_buffers[buffer_id]};
    bind_uniform_buffer(uniform_buffer.uniform_buffer_object, uniform_buffer.size, binding_point);
}

const UniformBuffer &get_uniform_buffer(UniformBufferId buffer_id);

void bind_uniform_buffer(const UniformBuffer &buffer, GLuint binding_point);
} // namespace Age::Gfx
