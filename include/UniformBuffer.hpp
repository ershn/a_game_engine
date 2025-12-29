#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

#include "OpenGL.hpp"

namespace Age::Gfx
{
std::size_t get_uniform_block_aligned_size(std::size_t block_size);

enum struct UniformBufferBinding : GLuint
{
};

enum struct UniformBufferRangeId : std::uint32_t
{
};

struct UniformBlock
{
    GLuint block_index{GL_INVALID_INDEX};
    UniformBufferBinding block_binding{};
};

bool is_uniform_block_defined(UniformBlock uniform_block);

struct UniformBufferRange
{
    GLuint buffer_object{};
    std::size_t offset{};
    std::size_t size{};
};

UniformBufferRangeId create_uniform_buffer_range(const UniformBufferRange &uniform_buffer_range);
void destroy_uniform_buffer_range(UniformBufferRangeId uniform_buffer_range_id);

template <typename TBlock>
struct UniformBuffer
{
    using Block = TBlock;

    GLuint buffer_object{};

    UniformBuffer(GLuint buffer_object)
        : buffer_object{buffer_object}
    {
        OGL::allocate_uniform_buffer(buffer_object, buffer_size());
    }

    static std::size_t block_size()
    {
        return sizeof(TBlock);
    }

    static std::size_t buffer_size()
    {
        return block_size();
    }

    UniformBufferRangeId create_range() const
    {
        return create_uniform_buffer_range({buffer_object, 0, buffer_size()});
    }

    void update(const TBlock &block) const
    {
        OGL::write_uniform_buffer(buffer_object, &block, sizeof(TBlock));
    }
};

template <typename TBlock>
struct UniformBuffer<TBlock[]>
{
    using Block = TBlock;

    GLuint buffer_object{};
    std::size_t size{};

    UniformBuffer(GLuint buffer_object, std::size_t size)
        : buffer_object{buffer_object}
        , size{size}
    {
        OGL::allocate_uniform_buffer(buffer_object, buffer_size());
    }

    static std::size_t block_size()
    {
        return get_uniform_block_aligned_size(sizeof(TBlock));
    }

    std::size_t buffer_size() const
    {
        return block_size() * size;
    }

    UniformBufferRangeId create_range() const
    {
        return create_uniform_buffer_range({buffer_object, 0, buffer_size()});
    }

    UniformBufferRangeId create_range(std::size_t start_index, std::size_t length) const
    {
        std::size_t block_size{this->block_size()};
        return create_uniform_buffer_range({buffer_object, start_index * block_size, block_size * length});
    }

    void update(std::size_t index, const TBlock &block) const
    {
        OGL::write_uniform_buffer(buffer_object, index * block_size(), &block, sizeof(TBlock));
    }
};

template <typename TBlock, std::size_t N>
struct UniformBuffer<TBlock[N]>
{
    using Block = TBlock;

    GLuint buffer_object{};

    UniformBuffer(GLuint buffer_object)
        : buffer_object{buffer_object}
    {
        OGL::allocate_uniform_buffer(buffer_object, buffer_size());
    }

    static std::size_t block_size()
    {
        return get_uniform_block_aligned_size(sizeof(TBlock));
    }

    static std::size_t buffer_size()
    {
        return block_size() * N;
    }

    UniformBufferRangeId create_range() const
    {
        return create_uniform_buffer_range({buffer_object, 0, buffer_size()});
    }

    UniformBufferRangeId create_range(std::size_t start_index, std::size_t length) const
    {
        std::size_t block_size{this->block_size()};
        return create_uniform_buffer_range({buffer_object, start_index * block_size, block_size * length});
    }

    void update(std::size_t index, const TBlock &block) const
    {
        OGL::write_uniform_buffer(buffer_object, index * block_size(), &block, sizeof(TBlock));
    }
};

template <typename TParam>
struct UniformBufferWriter
{
};

template <typename TParam>
struct UniformBufferWriter<UniformBuffer<TParam>>
{
    static_assert(std::is_array_v<TParam>, "Not an array UniformBuffer");

    const UniformBuffer<TParam> &uniform_buffer;
    std::unique_ptr<std::byte[]> write_buffer{};

    UniformBufferWriter(const UniformBuffer<TParam> &uniform_buffer)
        : uniform_buffer{uniform_buffer}
        , write_buffer{std::make_unique<std::byte[]>(uniform_buffer.buffer_size())}
    {
    }

    UniformBuffer<TParam>::Block &operator[](std::size_t index)
    {
        return *reinterpret_cast<UniformBuffer<TParam>::Block *>(&write_buffer[index * uniform_buffer.block_size()]);
    }

    void apply() const
    {
        OGL::write_uniform_buffer(uniform_buffer.buffer_object, write_buffer.get(), uniform_buffer.buffer_size());
    }
};

template <typename TBlock, typename... TParams>
UniformBuffer<TBlock> create_uniform_buffer(TParams &&...params)
{
    GLuint buffer_object{OGL::create_uniform_buffer()};
    return {buffer_object, std::forward<TParams>(params)...};
}

void init_uniform_buffer_system();

void bind_uniform_buffer_range(
    GLuint shader_program, UniformBlock &uniform_block, UniformBufferRangeId uniform_buffer_range_id
);
} // namespace Age::Gfx
