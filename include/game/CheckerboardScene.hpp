#pragma once

#include "Scene.hpp"

namespace Game
{
class CheckerBoardScene : public Age::App::IScene
{
    void init_entities() const override;
    void run_systems() const override;
};
} // namespace Game
