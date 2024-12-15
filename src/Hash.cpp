#include "Hash.hpp"

namespace Age::Util
{
// Moremur mixer (https://mostlymangling.blogspot.com/2019/12/stronger-better-morer-moremur-better.html)
std::size_t hash_mix(std::size_t hash)
{
    hash ^= hash >> 27;
    hash *= 0x3C79AC492BA7B653ULL;
    hash ^= hash >> 33;
    hash *= 0x1C69B3F74AC4AE35ULL;
    hash ^= hash >> 27;
    return hash;
}

// Based on SplitMix64 PRNG (https://prng.di.unimi.it/splitmix64.c)
void hash_combine(std::size_t &seed, std::size_t hash)
{
    seed = hash_mix(seed + hash + 0x9e3779b97f4a7c15ULL);
}
} // namespace Age::Util
