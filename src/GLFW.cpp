#include <iostream>

#include "ErrorHandling.hpp"
#include "GLFW.hpp"

namespace Age::GLFW
{
Initializer::Initializer()
    : _success{glfwInit() == GLFW_TRUE}
{
    LOG_ERROR_IF(_success == false, "GLFW initialization failed");
}

Initializer::~Initializer()
{
    glfwTerminate();
}

Initializer::operator bool()
{
    return _success;
}
} // namespace Age::GLFW
