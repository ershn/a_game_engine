#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include "error_handling.hpp"

namespace Age::Memory
{
template <typename T>
class BestFitLinearAllocator
{
  public:
    using value_type = T;
    using size_type = std::uint32_t;

    struct Slot
    {
        size_type offset{};
        size_type size{};
    };

    struct State
    {
        std::unique_ptr<std::byte[]> storage{};
        std::vector<Slot> slots{};

        State(size_type capacity)
            : storage{new std::byte[capacity * sizeof(T)]}
            , slots{{.offset = 0, .size = static_cast<size_type>(capacity * sizeof(T))}}
        {
        }
    };

  private:
    std::byte *_storage;
    std::vector<Slot> &_slots;

  public:
    explicit BestFitLinearAllocator(State &state)
        : _storage{state.storage.get()}
        , _slots{state.slots}
    {
    }

    template <typename U>
    BestFitLinearAllocator(const BestFitLinearAllocator<U> &allocator)
        : _storage{allocator._storage}
        , _slots{allocator._slots}
    {
    }

    bool operator==(const BestFitLinearAllocator<T> &other)
    {
        return _storage == other._storage && &_slots == &other._slots;
    }

    T *allocate(size_type size)
    {
        size *= sizeof(T);

        VBAIL_ERROR_IF(size == 0, nullptr, "requested allocation size is 0");

        auto selected_slot_it = _slots.end();
        size_type selected_slot_leading_space{std::numeric_limits<size_type>::max()};
        size_type selected_slot_following_space{std::numeric_limits<size_type>::max()};
        for (auto slot_it = _slots.begin(); slot_it != _slots.end(); ++slot_it)
        {
            size_type leading_space{alignof(T) - slot_it->offset % alignof(T)};
            size_type following_space{slot_it->size - leading_space - size};

            if (slot_it->size == size && leading_space == 0)
            {
                size_type offset{slot_it->offset};
                _slots.erase(slot_it);
                return reinterpret_cast<T *>(&_storage[offset]);
            }
            else if (
                slot_it->size - leading_space >= size &&
                leading_space + following_space < selected_slot_leading_space + selected_slot_following_space
            )
            {
                selected_slot_it = slot_it;
                selected_slot_leading_space = leading_space;
                selected_slot_following_space = following_space;
            }
        }

        VBAIL_ERROR_IF(selected_slot_it == _slots.end(), nullptr, "couldn't allocate the requested size: {}", size);

        size_type offset;
        if (selected_slot_leading_space == 0)
        {
            offset = selected_slot_it->offset;
            selected_slot_it->offset += size;
            selected_slot_it->size = selected_slot_following_space;
        }
        else if (selected_slot_following_space == 0)
        {
            offset = selected_slot_it->offset + selected_slot_leading_space;
            selected_slot_it->size = selected_slot_leading_space;
        }
        else
        {
            offset = selected_slot_it->offset + selected_slot_leading_space;
            selected_slot_it->size = selected_slot_leading_space;
            _slots.emplace(selected_slot_it + 1, offset + size, selected_slot_following_space);
        }
        return reinterpret_cast<T *>(&_storage[offset]);
    }

    void deallocate(T *array, size_type size)
    {
        size_type offset{static_cast<size_type>((array - reinterpret_cast<T *>(_storage)) * sizeof(T))};
        size *= sizeof(T);

        auto next_slot_it = std::find_if(_slots.begin(), _slots.end(), [&](Slot slot) {
            return slot.offset > offset;
        });

        bool has_prev_slot{next_slot_it != _slots.begin()};
        bool has_next_slot{next_slot_it != _slots.end()};
        if (has_prev_slot && has_next_slot)
        {
            auto prev_slot_it = next_slot_it - 1;
            bool contiguous_to_prev{prev_slot_it->offset + prev_slot_it->size == offset};
            bool contiguous_to_next{offset + size == next_slot_it->offset};
            if (contiguous_to_prev && contiguous_to_next)
            {
                prev_slot_it->size += size + next_slot_it->size;
                _slots.erase(next_slot_it);
            }
            else if (contiguous_to_prev)
            {
                prev_slot_it->size += size;
            }
            else if (contiguous_to_next)
            {
                next_slot_it->offset -= size;
                next_slot_it->size += size;
            }
            else
            {
                _slots.emplace(next_slot_it, offset, size);
            }
        }
        else if (has_prev_slot)
        {
            auto prev_slot_it = next_slot_it - 1;
            if (prev_slot_it->offset + prev_slot_it->size == offset)
                prev_slot_it->size += size;
            else
                _slots.emplace_back(offset, size);
        }
        else if (has_next_slot)
        {
            if (offset + size == next_slot_it->offset)
            {
                next_slot_it->offset -= size;
                next_slot_it->size += size;
            }
            else
            {
                _slots.emplace(next_slot_it, offset, size);
            }
        }
        else // _slots is empty
        {
            _slots.emplace_back(offset, size);
        }
    }
};
} // namespace Age::Memory
