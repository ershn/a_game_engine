#include <iostream>

#include "GLFW.hpp"

namespace Age::GLFW
{
Initializer::Initializer()
    : _success{glfwInit() == GLFW_TRUE}
{
    if (_success == false)
        std::cerr << "GLFW initialization failed" << '\n';
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
