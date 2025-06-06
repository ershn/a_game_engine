#include "Time.hpp"
#include "GLFW.hpp"

namespace Age::Time
{
namespace
{
double s_prev_frame_time;
double s_current_frame_time;
float s_delta_time;
} // namespace

float frame_time()
{
    return static_cast<float>(s_current_frame_time);
}

float delta_time()
{
    return s_delta_time;
}

float current_time()
{
    return static_cast<float>(glfwGetTime());
}

void init_frame_time()
{
    s_current_frame_time = glfwGetTime();
}

void update_frame_time()
{
    s_prev_frame_time = s_current_frame_time;
    s_current_frame_time = glfwGetTime();
    s_delta_time = static_cast<float>(s_current_frame_time - s_prev_frame_time);
}
} // namespace Age::Time
