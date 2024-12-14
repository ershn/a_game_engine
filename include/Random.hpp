#pragma once

#include "Hash.hpp"

namespace Age::Util
{
class Random
{
    std::size_t _state;

  public:
    Random(std::size_t seed);

    std::size_t next_integer();
};
} // namespace Age::Util
