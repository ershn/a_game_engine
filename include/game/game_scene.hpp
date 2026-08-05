#pragma once

#include "game/game_components.hpp"

namespace Game
{
struct Scene
{
    static constexpr std::size_t component_type_count{ComponentType::LAST_VALUE - ComponentType::FIRST_VALUE};
};
} // namespace Game
