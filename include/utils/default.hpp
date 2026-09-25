#pragma once

namespace Age::Utils
{
template <auto V>
struct Default
{
    decltype(V) value{V};

    constexpr Default() = default;

    constexpr Default(decltype(V) value)
        : value{value}
    {
    }

    template <decltype(V) W>
    constexpr Default(Default<W> other)
        : value{other.value}
    {
    }

    constexpr operator decltype(V)() const
    {
        return value;
    }
};
} // namespace Age::Utils
