#pragma once

#include <tuple>

namespace Age::Util
{
template <typename T, typename TTuple, typename = TTuple>
struct TupleElement
{
};

template <typename TTuple, typename T, typename... Ts>
struct TupleElement<T, std::tuple<T, Ts...>, TTuple>
{
    using QualifiedType = T;

    static const T &get(const TTuple &tuple)
    {
        return std::get<T>(tuple);
    }
};

template <typename TTuple, typename T, typename... Ts>
struct TupleElement<T, std::tuple<T *, Ts...>, TTuple>
{
    using QualifiedType = T *;

    static T &get(const TTuple &tuple)
    {
        return *std::get<T *>(tuple);
    }
};

template <typename TTuple, typename T, typename... Ts>
struct TupleElement<T, std::tuple<const T *, Ts...>, TTuple>
{
    using QualifiedType = const T *;

    static const T &get(const TTuple &tuple)
    {
        return *std::get<const T *>(tuple);
    }
};

template <typename TTuple, typename T, typename... Ts>
struct TupleElement<T, std::tuple<T &, Ts...>, TTuple>
{
    using QualifiedType = T &;

    static T &get(const TTuple &tuple)
    {
        return std::get<T &>(tuple);
    }
};

template <typename TTuple, typename T, typename... Ts>
struct TupleElement<T, std::tuple<const T &, Ts...>, TTuple>
{
    using QualifiedType = const T &;

    static const T &get(const TTuple &tuple)
    {
        return std::get<const T &>(tuple);
    }
};

template <typename TTuple, typename T, typename U, typename... Ts>
struct TupleElement<T, std::tuple<U, Ts...>, TTuple> : TupleElement<T, std::tuple<Ts...>, TTuple>
{
};

template <typename T, typename... Ts>
auto get_ref(const std::tuple<Ts...> &tuple) -> decltype(TupleElement<T, std::tuple<Ts...>>::get(tuple))
{
    return TupleElement<T, std::tuple<Ts...>>::get(tuple);
}
} // namespace Age::Util
