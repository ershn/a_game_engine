#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Age::GLFW
{
class Initializer
{
    bool _success;

  public:
    Initializer();
    ~Initializer();

    operator bool();
};
} // namespace Age::GLFW
