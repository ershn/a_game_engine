#pragma once

#include <cstdint>

#include "GLFW.hpp"

namespace Age::Gfx
{
using ViewportId = std::uint16_t;

inline constexpr ViewportId FULL_VIEWPORT_ID{0};
inline constexpr ViewportId USER_VIEWPORT_START_ID{1};

struct Viewport
{
    int origin_x{};
    int origin_y{};
    unsigned int width{};
    unsigned int height{};
};

bool has_framebuffer_size_changed();
void get_framebuffer_size(unsigned int &width, unsigned int &height);

void init_viewport_system(GLFWwindow *window);

void start_viewports_update();
void end_viewports_update();

void create_viewport(ViewportId viewport_id);

Viewport &get_viewport(ViewportId viewport_id);

const Viewport &use_viewport(ViewportId viewport_id);
} // namespace Age::Gfx
