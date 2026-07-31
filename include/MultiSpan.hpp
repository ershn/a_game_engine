#pragma once

#include <cstdint>
#include <tuple>
#include <utility>

#include "ErrorHandling.hpp"

namespace Age::Core
{
template <typename... Ts>
class MultiSpan
{
  public:
    using value_type = std::tuple<Ts &...>;
    using size_type = std::uint32_t;

    class iterator
    {
        const MultiSpan<Ts...> &_multi_span;
        size_type _index{};

      public:
        constexpr iterator(const MultiSpan<Ts...> &multi_span, size_type start_index)
            : _multi_span{multi_span}
            , _index{start_index}
        {
        }

        constexpr bool operator==(const iterator &it) const
        {
            return &_multi_span == &it._multi_span && _index == it._index;
        }

        constexpr bool operator!=(const iterator &it) const
        {
            return !(*this == it);
        }

        constexpr iterator &operator++()
        {
            ++_index;
            return *this;
        }

        constexpr value_type operator*() const
        {
            return _multi_span.get(_index);
        }
    };

  private:
    const std::tuple<Ts *...> _ptrs{};
    const size_type _size{};

  public:
    constexpr MultiSpan(size_type size, Ts *...ptrs)
        : _ptrs{ptrs...}
        , _size{size}
    {
    }

    constexpr value_type operator[](size_type index) const
    {
        VBAIL_ERROR_IF(
            index >= _size, get(_size == 0 ? 0 : _size - 1), "out of bounds index: {} (size is {})", index, _size
        );
        return get(index);
    }

    constexpr value_type front() const
    {
        LOG_ERROR_IF(_size == 0, "MultiSpan is empty");
        return get(0);
    }

    constexpr value_type back() const
    {
        VBAIL_ERROR_IF(_size == 0, get(0), "MultiSpan is empty");
        return get(_size - 1);
    }

    constexpr iterator begin() const
    {
        return iterator{*this, 0};
    }

    constexpr iterator cbegin() const
    {
        return iterator{*this, 0};
    }

    constexpr iterator end() const
    {
        return iterator{*this, _size};
    }

    constexpr iterator cend() const
    {
        return iterator{*this, _size};
    }

    constexpr bool empty() const
    {
        return _size == 0;
    }

    constexpr size_type size() const
    {
        return _size;
    }

  private:
    constexpr value_type get(size_type index) const
    {
        return get(index, std::index_sequence_for<Ts...>{});
    }

    template <std::size_t... Is>
    constexpr value_type get(size_type index, std::index_sequence<Is...>) const
    {
        return value_type{std::get<Is>(_ptrs)[index]...};
    }
};
} // namespace Age::Core
