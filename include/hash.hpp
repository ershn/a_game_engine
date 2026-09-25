#pragma once

#include <cstddef>

namespace Age::Core
{
std::size_t hash_mix(std::size_t hash);

void hash_combine(std::size_t &seed, std::size_t hash);
} // namespace Age::Core
