#pragma once

namespace Age::App
{
struct IScene
{
    virtual void init_entities() const = 0;
    virtual void run_systems() const = 0;
};
} // namespace Age::App
