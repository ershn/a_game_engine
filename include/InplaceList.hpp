#pragma once

#include <cstdint>
#include <utility>

#include "ErrorHandling.hpp"

namespace Age::Core
{
template <typename T, std::uint32_t N>
class InplaceList final
{
  public:
    using value_type = T;
    using size_type = std::uint32_t;

  private:
    alignas(T) std::byte _storage[sizeof(T) * N]{};
    size_type _size{};

  public:
    constexpr InplaceList()
    {
    }

    constexpr ~InplaceList()
    {
        for (T &element : *this)
            element.~T();
    }

    constexpr T &operator[](size_type index)
    {
        VBAIL_ERROR_IF(
            index >= _size, get(_size == 0 ? 0 : _size - 1), "out of bounds index: {} (size is {})", index, _size
        );
        return get(index);
    }

    constexpr const T &operator[](size_type index) const
    {
        VBAIL_ERROR_IF(
            index >= _size, get(_size == 0 ? 0 : _size - 1), "out of bounds index: {} (size is {})", index, _size
        );
        return get(index);
    }

    constexpr T &front()
    {
        LOG_ERROR_IF(_size == 0, "InplaceList is empty");
        return get(0);
    }

    constexpr const T &front() const
    {
        LOG_ERROR_IF(_size == 0, "InplaceList is empty");
        return get(0);
    }

    constexpr T &back()
    {
        VBAIL_ERROR_IF(_size == 0, get(0), "InplaceList is empty");
        return get(_size - 1);
    }

    constexpr const T &back() const
    {
        VBAIL_ERROR_IF(_size == 0, get(0), "InplaceList is empty");
        return get(_size - 1);
    }

    constexpr T *data()
    {
        return reinterpret_cast<T *>(_storage);
    }

    constexpr const T *data() const
    {
        return reinterpret_cast<const T *>(_storage);
    }

    constexpr T *begin()
    {
        return data();
    }

    constexpr const T *begin() const
    {
        return data();
    }

    constexpr const T *cbegin() const
    {
        return data();
    }

    constexpr T *end()
    {
        return data() + _size;
    }

    constexpr const T *end() const
    {
        return data() + _size;
    }

    constexpr const T *cend() const
    {
        return data() + _size;
    }

    constexpr bool empty() const
    {
        return _size == 0;
    }

    constexpr size_type size() const
    {
        return _size;
    }

    static constexpr size_type max_size()
    {
        return N;
    }

    static constexpr size_type capacity()
    {
        return N;
    }

    constexpr void clear()
    {
        for (T &element : *this)
            element.~T();

        _size = 0;
    }

    constexpr void push_back(const T &value)
    {
        BAIL_ERROR_IF(_size == N, "InplaceList already full: {}", N);

        new (address_of(_size++)) T{value};
    }

    constexpr void push_back(T &&value)
    {
        BAIL_ERROR_IF(_size == N, "InplaceList already full: {}", N);

        new (address_of(_size++)) T{std::move(value)};
    }

    template <typename... TArgs>
    constexpr T &emplace_back(TArgs &&...args)
    {
        VBAIL_ERROR_IF(_size == N, get(N == 0 ? N : N - 1), "InplaceList already full: {}", N);

        return *new (address_of(_size++)) T{std::forward<TArgs>(args)...};
    }

    constexpr void pop_back()
    {
        BAIL_ERROR_IF(_size == 0, "InplaceList is empty");

        back().~T();
        --_size;
    }

  private:
    constexpr std::byte *address_of(size_type index)
    {
        return &_storage[sizeof(T) * index];
    }

    constexpr const std::byte *address_of(size_type index) const
    {
        return &_storage[sizeof(T) * index];
    }

    constexpr T &get(size_type index)
    {
        return *(data() + index);
    }

    constexpr const T &get(size_type index) const
    {
        return *(data() + index);
    }
};
} // namespace Age::Core
