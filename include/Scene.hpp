#pragma once

namespace Age::App
{
struct IScene
{
    virtual void init() const = 0;
    virtual void update() const = 0;
};
} // namespace Age::App
