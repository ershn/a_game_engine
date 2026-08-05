#pragma once

#include <cstdint>

#include "glfw.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx
{
enum struct FramebufferId : std::uint16_t
{
};

inline constexpr FramebufferId SYSTEM_FRAMEBUFFER_ID{1};

struct Framebuffer
{
    GLuint framebuffer{};
    std::uint32_t width{};
    std::uint32_t height{};
};

void init_framebuffer_system(GLFWwindow *window);

void update_system_framebuffer_size();
bool has_system_framebuffer_size_changed();

Framebuffer &get_framebuffer(FramebufferId framebuffer_id);

const Framebuffer &use_framebuffer(FramebufferId framebuffer_id);
} // namespace Age::Gfx
