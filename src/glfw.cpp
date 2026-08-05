#include <glad/glad.h>

#include "error_handling.hpp"
#include "glfw.hpp"

namespace Age::GLFW
{
namespace
{
void error_callback(int error, const char *description)
{
    Core::log_error("{}", std::string_view{description});
}
} // namespace

bool init()
{
    glfwSetErrorCallback(error_callback);

    if (glfwInit() == false)
        VBAIL_ERROR(false, "GLFW initialization failed");

    return true;
}

void terminate()
{
    glfwTerminate();
}

GLFWwindow *create_window(unsigned int width, unsigned int height, const std::string &title)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, true);

    GLFWwindow *window{glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr)};
    LOG_ERROR_IF(window == nullptr, "window/context creation failed");

    return window;
}

void destroy_window(GLFWwindow *window)
{
    glfwDestroyWindow(window);
}

bool load_graphics_api(GLFWwindow *window)
{
    glfwMakeContextCurrent(window);

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
        VBAIL_ERROR(false, "OpenGL loading failed");

    Core::log_info("loaded OpenGL {}.{}", GLVersion.major, GLVersion.minor);
    return true;
}

void poll_events()
{
    glfwPollEvents();
}
} // namespace Age::GLFW
