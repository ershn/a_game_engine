#pragma once

#include "game/game_scene.hpp"

namespace Game
{
struct ProjectedLightScene : Scene
{
    static void init();
    static void update();
    static void render();
};
} // namespace Game
