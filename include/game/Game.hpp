#pragma once

#include "Components.hpp"

namespace Game
{
struct Scene
{
    static constexpr std::size_t component_type_count{
        ComponentType::LAST_VALUE - static_cast<std::size_t>(Age::Core::ComponentType::LAST_VALUE)
    };
};
} // namespace Game
