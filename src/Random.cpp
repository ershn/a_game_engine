#include "Random.hpp"

namespace Age::Util
{
Random::Random(std::size_t seed)
    : _state{seed}
{
}

std::size_t Random::next_integer()
{
    _state += 0x9e3779b97f4a7c15ULL;
    return hash_mix(_state);
}
} // namespace Age::Util
