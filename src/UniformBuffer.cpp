#include <vector>

#include "IdGenerator.hpp"
#include "UniformBuffer.hpp"

namespace Age::Gfx
{
namespace
{
GLuint s_uniform_buffer_binding_count;
std::size_t s_uniform_buffer_offset_alignment;

std::vector<std::uint32_t> s_binding_use_counts;
std::vector<UniformBufferRangeId> s_binding_uniform_buffer_ranges;

Util::IdGenerator<UniformBufferRangeId> s_uniform_buffer_range_id_generator{UniformBufferRangeId{1}};
std::vector<UniformBufferRange> s_uniform_buffer_ranges;
std::vector<UniformBufferBinding> s_uniform_buffer_range_bindings;

constexpr std::size_t to_index(UniformBufferBinding binding)
{
    return static_cast<std::size_t>(binding) - 1;
}

constexpr UniformBufferBinding to_binding(std::size_t index)
{
    return static_cast<UniformBufferBinding>(index + 1);
}

constexpr std::size_t to_index(UniformBufferRangeId id)
{
    return static_cast<std::size_t>(id) - 1;
}

constexpr UniformBufferRangeId to_range_id(std::size_t index)
{
    return static_cast<UniformBufferRangeId>(index + 1);
}

UniformBufferBinding get_least_used_uniform_buffer_binding()
{
    if (s_binding_use_counts[0] == 0)
        return to_binding(0);

    std::size_t least_used_binding_index{0};
    std::size_t max_binding_index{s_uniform_buffer_binding_count};
    for (std::size_t binding_index{1}; binding_index < max_binding_index; ++binding_index)
    {
        std::uint32_t use_count{s_binding_use_counts[binding_index]};

        if (use_count == 0)
            return to_binding(binding_index);
        else if (use_count < s_binding_use_counts[least_used_binding_index])
            least_used_binding_index = binding_index;
    }
    return to_binding(least_used_binding_index);
}

void bind_uniform_buffer_range(UniformBufferRangeId uniform_buffer_range_id, UniformBufferBinding binding)
{
    std::size_t range_index{to_index(uniform_buffer_range_id)};
    std::size_t binding_index{to_index(binding)};
    UniformBufferRange &uniform_buffer_range{s_uniform_buffer_ranges[range_index]};

    OGL::bind_uniform_buffer_range(
        static_cast<GLuint>(binding_index),
        uniform_buffer_range.buffer_object,
        uniform_buffer_range.offset,
        uniform_buffer_range.size
    );

    s_uniform_buffer_range_bindings[range_index] = binding;
    s_binding_uniform_buffer_ranges[binding_index] = uniform_buffer_range_id;
}

void unbind_uniform_buffer_range(UniformBufferBinding binding)
{
    std::size_t binding_index{to_index(binding)};
    UniformBufferRangeId uniform_buffer_range_id{s_binding_uniform_buffer_ranges[binding_index]};
    std::size_t range_index{to_index(uniform_buffer_range_id)};

    if (uniform_buffer_range_id == UniformBufferRangeId{})
        return;

    s_uniform_buffer_range_bindings[range_index] = {};
    s_binding_uniform_buffer_ranges[binding_index] = UniformBufferRangeId{};
}

void bind_uniform_block(GLuint shader_program, UniformBlock &uniform_block, UniformBufferBinding binding)
{
    UniformBufferBinding &block_binding{uniform_block.block_binding};

    if (block_binding == binding)
        return;

    if (block_binding != UniformBufferBinding{})
        --s_binding_use_counts[to_index(block_binding)];

    OGL::bind_uniform_block(shader_program, uniform_block.block_index, static_cast<GLuint>(to_index(binding)));

    block_binding = binding;
    ++s_binding_use_counts[to_index(block_binding)];
}
} // namespace

std::size_t get_uniform_block_aligned_size(std::size_t block_size)
{
    return block_size + s_uniform_buffer_offset_alignment - (block_size % s_uniform_buffer_offset_alignment);
}

bool is_uniform_block_defined(UniformBlock uniform_block)
{
    return uniform_block.block_index != GL_INVALID_INDEX;
}

UniformBufferRangeId create_uniform_buffer_range(const UniformBufferRange &uniform_buffer_range)
{
    UniformBufferRangeId id{s_uniform_buffer_range_id_generator.generate()};
    std::size_t index{to_index(id)};

    if (index == s_uniform_buffer_ranges.size())
    {
        s_uniform_buffer_ranges.emplace_back(uniform_buffer_range);
        s_uniform_buffer_range_bindings.emplace_back();
    }
    else
    {
        s_uniform_buffer_ranges[index] = uniform_buffer_range;
    }
    return id;
}

void destroy_uniform_buffer_range(UniformBufferRangeId uniform_buffer_range_id)
{
    s_uniform_buffer_range_id_generator.destroy(uniform_buffer_range_id);
}

void init_uniform_buffer_system()
{
    s_uniform_buffer_binding_count = static_cast<GLuint>(OGL::get_integer(GL_MAX_UNIFORM_BUFFER_BINDINGS));
    s_uniform_buffer_offset_alignment = static_cast<std::size_t>(OGL::get_integer(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT));

    s_binding_use_counts.resize(s_uniform_buffer_binding_count);
    s_binding_uniform_buffer_ranges.resize(s_uniform_buffer_binding_count);

    s_uniform_buffer_ranges.reserve(64);
    s_uniform_buffer_range_bindings.reserve(64);
}

void bind_uniform_buffer_range(
    GLuint shader_program, UniformBlock &uniform_block, UniformBufferRangeId uniform_buffer_range_id
)
{
    UniformBufferBinding buffer_binding{s_uniform_buffer_range_bindings[to_index(uniform_buffer_range_id)]};

    if (buffer_binding == UniformBufferBinding{})
    {
        buffer_binding = get_least_used_uniform_buffer_binding();
        unbind_uniform_buffer_range(buffer_binding);
        bind_uniform_buffer_range(uniform_buffer_range_id, buffer_binding);
        bind_uniform_block(shader_program, uniform_block, buffer_binding);
    }
    else
    {
        bind_uniform_block(shader_program, uniform_block, buffer_binding);
    }
}
} // namespace Age::Gfx
