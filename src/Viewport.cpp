#include <limits>
#include <vector>

#include "OpenGL.hpp"
#include "Viewport.hpp"

namespace Age::Gfx
{
namespace
{
bool s_framebuffer_size_changed{};
int s_framebuffer_width{};
int s_framebuffer_height{};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    s_framebuffer_width = width;
    s_framebuffer_height = height;
    s_framebuffer_size_changed = true;
}

std::vector<Viewport> s_viewports{};
ViewportId s_used_viewport_id{std::numeric_limits<ViewportId>::max()};
} // namespace

bool has_framebuffer_size_changed()
{
    return s_framebuffer_size_changed;
}

void get_framebuffer_size(unsigned int &width, unsigned int &height)
{
    width = static_cast<unsigned int>(s_framebuffer_width);
    height = static_cast<unsigned int>(s_framebuffer_height);
}

void init_viewport_system(GLFWwindow *window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwGetFramebufferSize(window, &s_framebuffer_width, &s_framebuffer_height);
    s_framebuffer_size_changed = true;

    s_viewports.reserve(8);
    s_viewports.emplace_back(
        0, 0, static_cast<unsigned int>(s_framebuffer_width), static_cast<unsigned int>(s_framebuffer_height)
    );
}

void start_viewports_update()
{
    if (s_framebuffer_size_changed == false)
        return;

    Viewport &full_viewport{s_viewports[FULL_VIEWPORT_ID]};
    full_viewport.width = static_cast<unsigned int>(s_framebuffer_width);
    full_viewport.height = static_cast<unsigned int>(s_framebuffer_height);

    s_used_viewport_id = std::numeric_limits<ViewportId>::max();
}

void end_viewports_update()
{
    s_framebuffer_size_changed = false;
}

void create_viewport(ViewportId viewport_id)
{
    if (viewport_id >= s_viewports.size())
        s_viewports.resize(viewport_id + 1);
}

Viewport &get_viewport(ViewportId viewport_id)
{
    return s_viewports[viewport_id];
}

const Viewport &use_viewport(ViewportId viewport_id)
{
    const Viewport &viewport{s_viewports[viewport_id]};

    if (viewport_id != s_used_viewport_id)
    {
        glViewport(
            viewport.origin_x,
            viewport.origin_y,
            static_cast<GLsizei>(viewport.width),
            static_cast<GLsizei>(viewport.height)
        );

        s_used_viewport_id = viewport_id;
    }

    return viewport;
}
} // namespace Age::Gfx
