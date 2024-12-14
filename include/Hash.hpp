#pragma once

#include <cstddef>
#include <span>

namespace Age::Util
{
std::size_t hash_mix(std::size_t hash);

void hash_combine(std::size_t &seed, std::size_t hash);

std::size_t hash_combine(std::span<std::size_t> hashes);
} // namespace Age::Util
