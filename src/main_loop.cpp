#include "main_loop.hpp"

namespace Age::Core
{
namespace
{
bool s_is_exit_requested{};
} // namespace

void request_exit()
{
    s_is_exit_requested = true;
}

bool is_exit_requested()
{
    return s_is_exit_requested;
}

GLFWwindow *create_window(unsigned int width, unsigned int height, const std::string &title)
{
    if (GLFW::init() == false)
        return nullptr;

    GLFWwindow *window{GLFW::create_window(width, height, title)};
    if (window == nullptr)
    {
        GLFW::terminate();
        return nullptr;
    }

    if (GLFW::load_graphics_api(window) == false)
    {
        GLFW::terminate();
        return nullptr;
    }

    return window;
}

void destroy_window(GLFWwindow *window)
{
    GLFW::destroy_window(window);

    GLFW::terminate();
}
} // namespace Age::Core
