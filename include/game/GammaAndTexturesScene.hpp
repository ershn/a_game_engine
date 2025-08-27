#pragma once

#include "Scene.hpp"

namespace Game
{
class GammaAndTexturesScene : public Age::App::IScene
{
    void init() const override;
    void update() const override;
};
} // namespace Game
