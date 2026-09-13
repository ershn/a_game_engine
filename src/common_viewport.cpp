#include <vector>

#include "common_viewport.hpp"
#include "id_generator.hpp"

namespace Age::Gfx
{
namespace
{
Util::IdGenerator<ViewportId> s_viewport_id_generator{ViewportId{2}};
std::vector<Viewport> s_viewports{Viewport{.norm_rect{{0.0f, 0.0f}, {1.0f, 1.0f}}}};

constexpr std::size_t to_index(ViewportId id)
{
    return static_cast<std::size_t>(id) - 1;
}
} // namespace

void init_viewport_system()
{
    s_viewports.reserve(8);
}

ViewportId create_viewport(const Math::Rectangle &norm_rect)
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

Math::RectangleI calc_viewport_rect(const Viewport &viewport, const Math::Vector2U &framebuffer_size)
{
    return Math::RectangleI{Math::round(Math::scale(viewport.norm_rect, framebuffer_size))};
}
} // namespace Age::Gfx
