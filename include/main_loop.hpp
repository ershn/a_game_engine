#pragma once

#include <concepts>
#include <cstddef>

#include "default_meshes.hpp"
#include "ecs.hpp"
#include "glfw.hpp"
#include "input.hpp"
#include "logging.hpp"
#include "rendering.hpp"
#include "time.hpp"

namespace Age::Core
{
template <typename T>
concept IsScene = requires {
    { T::component_type_count } -> std::convertible_to<std::size_t>;
    T::init();
    T::update();
    T::render();
};

void request_exit();
bool is_exit_requested();

GLFWwindow *create_window(unsigned int width, unsigned int height, const std::string &title);
void destroy_window(GLFWwindow *window);

template <IsScene T>
void run_engine()
{
    Core::init_logging();

    GLFWwindow *window{create_window(1280, 720, "Age")};
    if (window == nullptr)
        return;

    Core::init_ecs(T::component_type_count);
    Input::init_input_system(window);
    Gfx::init_rendering_system(window);

    Gfx::load_primitive_meshes();
    T::init();

    GLFW::poll_events();

    Input::init_input_state();
    Time::init_frame_time();
    while (Input::should_window_close() == false && is_exit_requested() == false)
    {
        Time::update_frame_time();

        T::update();

        Gfx::render<T::render>();

        GLFW::poll_events();

        Input::update_input_state();
        Gfx::update_render_state();
    }

    destroy_window(window);
}
} // namespace Age::Core
