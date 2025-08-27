#pragma once

#include "Scene.hpp"

namespace Game
{
struct DoubleProjectionScene : public Age::App::IScene
{
    void init() const override;
    void update() const override;
};
} // namespace Game
