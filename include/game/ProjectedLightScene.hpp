#pragma once

#include "Scene.hpp"

namespace Game
{
struct ProjectedLightScene : public Age::App::IScene
{
    void init() const override;
    void update() const override;
    void render() const override;
};
} // namespace Game
