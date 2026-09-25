#pragma once

#include <cstddef>

namespace Age::Core
{
class Random
{
    std::size_t _state;

  public:
    Random(std::size_t seed);

    std::size_t next_integer();
};
} // namespace Age::Core
