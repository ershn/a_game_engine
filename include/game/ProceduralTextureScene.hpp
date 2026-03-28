#pragma once

#include "Scene.hpp"

namespace Game
{
struct ProceduralTextureScene : public Age::App::IScene
{
    void init() const override;
    void update() const override;
};
} // namespace Game
