#pragma once

#include <vector>

namespace Age::Util
{
template <typename T> class IdGenerator
{
    T _next_id{};
    std::vector<T> _free_ids{};

  public:
    IdGenerator(T first_id, std::size_t free_list_capacity = 128)
        : _next_id{first_id}
        , _free_ids{}
    {
        _free_ids.reserve(free_list_capacity);
    }

    T generate()
    {
        if (_free_ids.empty())
        {
            return _next_id++;
        }
        else
        {
            T id{_free_ids.back()};
            _free_ids.pop_back();
            return id;
        }
    }

    void destroy(T id)
    {
        _free_ids.emplace_back(id);
    }
};
} // namespace Age::Util
