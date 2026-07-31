#pragma once

#include <type_traits>

namespace Age::Util
{
template <typename... TArgs>
struct TypePack
{
};

template <typename T>
struct TypeArgs
{
};

template <template <typename...> typename T, typename... TArgs>
struct TypeArgs<T<TArgs...>>
{
    using TypePack = TypePack<TArgs...>;
};

template <typename T>
constexpr std::underlying_type_t<T> to_underlying(T enum_value)
{
    return static_cast<std::underlying_type_t<T>>(enum_value);
}
} // namespace Age::Util
