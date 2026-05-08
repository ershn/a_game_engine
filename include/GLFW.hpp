#pragma once

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Age::GLFW
{
bool init();
void terminate();

GLFWwindow *create_window(unsigned int width, unsigned int height, const std::string &title);
void destroy_window(GLFWwindow *window);

bool load_graphics_api(GLFWwindow *window);

void poll_events();
} // namespace Age::GLFW
