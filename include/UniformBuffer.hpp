#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "Components.hpp"
#include "OpenGL.hpp"

namespace Age::Gfx
{
std::size_t get_uniform_buffer_alignment();
std::size_t get_uniform_block_alignment(std::size_t block_size);

using UniformBufferId = std::uint16_t;
using UniformBlockIndex = std::uint32_t;
using UniformBlockCount = std::uint32_t;

struct UniformBuffer
{
    GLuint uniform_buffer_object{};
    std::size_t size{};

    UniformBuffer(std::size_t size);
};

struct UniformBufferRange
{
    GLuint uniform_buffer_object{};
    std::size_t offset{};
    std::size_t size{};
};

struct UniformBufferRangeBind
{
    static constexpr auto TYPE{Core::ComponentType::UNIFORM_BUFFER_RANGE_BIND};

    UniformBufferRange buffer_range{};
    GLuint binding_point{};
};

template <typename TBlock>
struct UniformBufferBlock
{
    static constexpr auto TYPE{Core::ComponentType::UNIFORM_BUFFER_BLOCK};

    GLuint uniform_buffer_object{};
    UniformBlockIndex block_index{};

    std::size_t get_buffer_offset() const
    {
        return block_index == 0 ? 0 : block_index * get_uniform_block_alignment(sizeof(TBlock));
    }

    UniformBufferRange get_buffer_range() const
    {
        return {.uniform_buffer_object{uniform_buffer_object}, .offset{get_buffer_offset()}, .size{sizeof(TBlock)}};
    }

    const TBlock &operator=(const TBlock &block) const
    {
        OGL::write_uniform_buffer(uniform_buffer_object, get_buffer_offset(), &block, sizeof(TBlock));
        return block;
    }
};

template <typename TBlock>
struct ScalarUniformBuffer : public UniformBuffer
{
    using Block = TBlock;

    ScalarUniformBuffer()
        : UniformBuffer{sizeof(TBlock)}
    {
    }

    UniformBufferBlock<TBlock> get_block() const
    {
        return {uniform_buffer_object, 0};
    }
};

template <typename TBlock>
struct ArrayUniformBuffer : public UniformBuffer
{
    using Block = TBlock;

    ArrayUniformBuffer(UniformBlockCount block_count)
        : UniformBuffer{get_uniform_block_alignment(sizeof(TBlock)) * block_count}
    {
    }

    UniformBufferBlock<TBlock> get_block(UniformBlockIndex index) const
    {
        return {uniform_buffer_object, index};
    }
};

template <typename TArrayUniformBuffer>
struct ArrayUniformBufferWriter
{
    using Block = TArrayUniformBuffer::Block;

    const TArrayUniformBuffer &uniform_buffer;
    std::unique_ptr<std::byte[]> write_buffer;

    ArrayUniformBufferWriter(const TArrayUniformBuffer &uniform_buffer)
        : uniform_buffer{uniform_buffer}
        , write_buffer{std::make_unique<std::byte[]>(uniform_buffer.size)}
    {
    }

    Block &operator[](UniformBlockIndex index)
    {
        return *static_cast<Block *>(
            static_cast<void *>(&write_buffer[index * get_uniform_block_alignment(sizeof(Block))]));
    }

    void apply() const
    {
        OGL::write_uniform_buffer(uniform_buffer.uniform_buffer_object, write_buffer.get(), uniform_buffer.size);
    }
};

extern std::vector<std::unique_ptr<UniformBuffer>> g_uniform_buffers;

void init_uniform_buffer_system();

template <typename TUniformBuffer, typename... TArgs>
TUniformBuffer &create_uniform_buffer(UniformBufferId buffer_id, TArgs &&...buffer_args)
{
    if (buffer_id >= g_uniform_buffers.size())
        g_uniform_buffers.resize(buffer_id + 1);

    g_uniform_buffers[buffer_id] = std::make_unique<TUniformBuffer>(std::forward<TArgs>(buffer_args)...);
    return static_cast<TUniformBuffer &>(*g_uniform_buffers[buffer_id]);
}

const UniformBuffer &get_uniform_buffer(UniformBufferId buffer_id);

void bind_uniform_buffer(GLuint binding_point, const UniformBufferRange &buffer_range);
void bind_uniform_buffer(const UniformBufferRangeBind &buffer_bind);
} // namespace Age::Gfx
