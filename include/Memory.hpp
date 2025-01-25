#pragma once

#include <cstddef>
#include <vector>

namespace Age::Memory
{
template <std::size_t N>
constexpr std::size_t padding_to(std::size_t size)
{
    return (N - size % N) % N;
}

template <std::size_t AllocSize, std::size_t ChunkSize>
class PoolAllocator
{
    static_assert(ChunkSize <= AllocSize, "ChunkSize must be less than or equal to AllocSize");
    static_assert(AllocSize % ChunkSize == 0, "AllocSize must be divisible by ChunkSize");

    constexpr static std::size_t CHUNK_COUNT_PER_PAGE{AllocSize / ChunkSize};

    std::byte *_last_allocated_page{nullptr};
    std::size_t _page_next_free_chunk_index{};
    std::vector<void *> _returned_chunks{};

  public:
    PoolAllocator()
    {
        _returned_chunks.reserve(16);
    }

    void *get_chunk()
    {
        if (_returned_chunks.size() > 0)
        {
            void *chunk{_returned_chunks.back()};
            _returned_chunks.pop_back();
            return chunk;
        }

        if (_last_allocated_page == nullptr || _page_next_free_chunk_index == CHUNK_COUNT_PER_PAGE)
        {
            _last_allocated_page = new std::byte[AllocSize];
            _page_next_free_chunk_index = 0;
        }

        return &_last_allocated_page[_page_next_free_chunk_index++ * ChunkSize];
    }

    void return_chunk(void *chunk_ptr)
    {
        _returned_chunks.emplace_back(chunk_ptr);
    }
};
} // namespace Age::Memory
