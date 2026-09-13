#pragma once

#include "game/game_scene.hpp"

namespace Game
{
struct ProceduralTextureScene : Scene
{
    static void init();
    static void update_render_state();
    static void update();
    static void render();
};
} // namespace Game
