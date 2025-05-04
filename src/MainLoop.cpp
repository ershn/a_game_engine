#include "MainLoop.hpp"
#include "ECS.hpp"
#include "ErrorHandling.hpp"
#include "GLFW.hpp"
#include "Input.hpp"
#include "Logging.hpp"
#include "MeshInstances.hpp"
#include "Rendering.hpp"
#include "Time.hpp"

namespace Age::Core
{
namespace
{
bool s_is_exit_requested{};

void error_callback(int error, const char *description)
{
    Core::log_error("{}", std::string_view{description});
}
} // namespace

void exit()
{
    s_is_exit_requested = true;
}

void run_engine(const App::Definitions &definitions, const App::IScene &scene)
{
    Core::init_logging();

    GLFW::Initializer glfw_initializer{};
    if (glfw_initializer == false)
        return;

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window{glfwCreateWindow(1280, 720, "Age", nullptr, nullptr)};
    BAIL_ERROR_IF(window == nullptr, "window/context creation failed");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        BAIL_ERROR("OpenGL loading failed");

    Core::log_info("loaded OpenGL {}.{}", GLVersion.major, GLVersion.minor);

    Core::init_ecs(definitions);
    Input::init_input_system(window);
    Gfx::init_rendering_system(window);

    Gfx::load_primitive_meshes();
    scene.init_entities();

    Time::init_frame_time();
    while (Input::is_exit_requested() == false && s_is_exit_requested == false)
    {
        Time::update_frame_time();
        scene.run_systems();
        Gfx::render();
        Input::update_input_state();
    }

    glfwDestroyWindow(window);
}
} // namespace Age::Core
