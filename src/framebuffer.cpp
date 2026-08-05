#include <vector>

#include "framebuffer.hpp"
#include "id_generator.hpp"

namespace Age::Gfx
{
namespace
{
int s_framebuffer_real_width{};
int s_framebuffer_real_height{};
bool s_framebuffer_real_size_changed{};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    s_framebuffer_real_width = width;
    s_framebuffer_real_height = height;

    s_framebuffer_real_size_changed = true;
}

Util::IdGenerator<FramebufferId> s_framebuffer_id_generator{FramebufferId{2}};
std::vector<Framebuffer> s_framebuffers{Framebuffer{.framebuffer = 0}};

bool s_system_framebuffer_size_changed{};

constexpr std::size_t to_index(FramebufferId id)
{
    return static_cast<std::size_t>(id) - 1;
}
} // namespace

void init_framebuffer_system(GLFWwindow *window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Framebuffer &system_framebuffer{get_framebuffer(SYSTEM_FRAMEBUFFER_ID)};
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    system_framebuffer.width = static_cast<std::uint32_t>(width);
    system_framebuffer.height = static_cast<std::uint32_t>(height);

    s_system_framebuffer_size_changed = true;
}

void update_system_framebuffer_size()
{
    if (s_framebuffer_real_size_changed)
    {
        s_framebuffer_real_size_changed = false;

        Framebuffer &system_framebuffer{get_framebuffer(SYSTEM_FRAMEBUFFER_ID)};
        system_framebuffer.width = static_cast<std::uint32_t>(s_framebuffer_real_width);
        system_framebuffer.height = static_cast<std::uint32_t>(s_framebuffer_real_height);

        s_system_framebuffer_size_changed = true;
    }
    else
    {
        s_system_framebuffer_size_changed = false;
    }
}

bool has_system_framebuffer_size_changed()
{
    return s_system_framebuffer_size_changed;
}

Framebuffer &get_framebuffer(FramebufferId framebuffer_id)
{
    return s_framebuffers[to_index(framebuffer_id)];
}

const Framebuffer &use_framebuffer(FramebufferId framebuffer_id)
{
    // TODO
    return get_framebuffer(framebuffer_id);
}
} // namespace Age::Gfx
