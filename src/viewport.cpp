#include <cmath>
#include <vector>

#include "id_generator.hpp"
#include "viewport.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx
{
namespace
{
Util::IdGenerator<ViewportId> s_viewport_id_generator{ViewportId{2}};
std::vector<Viewport> s_viewports{Viewport{.norm_rect{.position{0.0f, 0.0f}, .size{1.0f, 1.0f}}}};

RectangleI s_current_viewport_pixel_rect{};

constexpr std::size_t to_index(ViewportId id)
{
    return static_cast<std::size_t>(id) - 1;
}
} // namespace

void init_viewport_system()
{
    s_viewports.reserve(8);
}

ViewportId create_viewport(const Rectangle &norm_rect)
{
    ViewportId viewport_id{s_viewport_id_generator.generate()};

    std::size_t viewport_index{to_index(viewport_id)};
    if (viewport_index >= s_viewports.size())
        s_viewports.resize(viewport_index + 1);

    Viewport &viewport{s_viewports[viewport_index]};
    viewport.norm_rect = norm_rect;

    return viewport_id;
}

Viewport &get_viewport(ViewportId viewport_id)
{
    return s_viewports[to_index(viewport_id)];
}

RectangleI calc_viewport_pixel_rect(const Viewport &viewport, const Framebuffer &framebuffer)
{
    const Rectangle &norm_rect{viewport.norm_rect};
    return RectangleI{
        .position{
            static_cast<int>(std::lround(norm_rect.position.x * framebuffer.width)),
            static_cast<int>(std::lround(norm_rect.position.y * framebuffer.height))
        },
        .size{
            static_cast<int>(std::lround(norm_rect.size.x * framebuffer.width)),
            static_cast<int>(std::lround(norm_rect.size.y * framebuffer.height))
        }
    };
}

void use_viewport_pixel_rect(const RectangleI &pixel_rect)
{
    if (pixel_rect != s_current_viewport_pixel_rect)
    {
        OGL::set_viewport(pixel_rect);
        s_current_viewport_pixel_rect = pixel_rect;
    }
}
} // namespace Age::Gfx
