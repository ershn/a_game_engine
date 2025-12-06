#pragma once

#include <type_traits>

namespace Age::Util
{
template <typename T>
constexpr std::underlying_type_t<T> to_underlying(T enum_value)
{
    return static_cast<std::underlying_type_t<T>>(enum_value);
}
} // namespace Age::Util
