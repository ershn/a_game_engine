#pragma once

#include <cstddef>

namespace Game
{
std::size_t get_component_type_count();

void init_entities();

void run_systems();
} // namespace Game
