#pragma once

#include "Scene.hpp"

namespace Game
{
struct CubePointLightScene : public Age::App::IScene
{
    void init() const override;
    void update() const override;
};
} // namespace Game
