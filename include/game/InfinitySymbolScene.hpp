#pragma once

#include "Scene.hpp"

namespace Game
{
struct InfinitySymbolScene : public Age::App::IScene
{
    void init() const override;
    void update() const override;
};
} // namespace Game
