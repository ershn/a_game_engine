#include <iostream>

#include "ECS.hpp"
#include "GLFW.hpp"
#include "Input.hpp"
#include "MainLoop.hpp"
#include "Rendering.hpp"
#include "Time.hpp"

#include "MeshInstances.hpp"

#include "game/Game.hpp"

namespace Age::Core
{
namespace
{
bool s_is_exit_requested{};

void error_callback(int error, const char *description)
{
    std::cerr << "Error: " << description << std::endl;
}
} // namespace

void exit()
{
    s_is_exit_requested = true;
}

void run_engine()
{
    GLFW::Initializer glfw_initializer{};
    if (glfw_initializer == false)
        return;

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window{glfwCreateWindow(1280, 720, "Age", nullptr, nullptr)};
    if (window == nullptr)
    {
        std::cerr << "Window/context creation failed" << '\n';
        return;
    }

    glfwMakeContextCurrent(window);

    int version{gladLoadGL(glfwGetProcAddress)};
    if (version == 0)
    {
        std::cerr << "OpenGL loading failed" << '\n';
        return;
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << '\n';

    Core::init_ecs();
    Input::init_input_system(window);
    Gfx::init_rendering_system(window);

    Gfx::load_primitive_models();
    Game::init_entities();

    Time::init_frame_time();
    while (Input::is_exit_requested() == false && s_is_exit_requested == false)
    {
        Time::update_frame_time();
        Game::run_systems();
        Gfx::render();
        Input::update_input_state();
    }

    glfwDestroyWindow(window);
}
} // namespace Age::Core
