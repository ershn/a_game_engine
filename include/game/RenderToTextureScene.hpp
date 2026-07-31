#pragma once

#include "Game.hpp"

namespace Game
{
struct RenderToTextureScene : Scene
{
    static void init();
    static void update();
    static void render();
};
} // namespace Game
