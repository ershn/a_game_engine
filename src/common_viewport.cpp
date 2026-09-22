#include <vector>

#include "common_viewport.hpp"
#include "id_generator.hpp"

namespace Age::Gfx
{
namespace
{
Util::IdGenerator<ViewportId> s_viewport_id_generator{ViewportId{2}};
std::vector<Viewport> s_viewports{Viewport{.rect{{0.0f, 0.0f}, {1.0f, 1.0f}}}};

constexpr std::size_t to_index(ViewportId id)
{
    return static_cast<std::size_t>(id) - 1;
}
} // namespace

void init_viewport_storage()
{
    s_viewports.reserve(8);
}

ViewportId create_viewport(const Viewport &viewport)
{
    ViewportId viewport_id{s_viewport_id_generator.generate()};

    std::size_t viewport_index{to_index(viewport_id)};
    if (viewport_index >= s_viewports.size())
        s_viewports.resize(viewport_index + 1);

    s_viewports[viewport_index] = viewport;

    return viewport_id;
}

Math::Vector2I mapped_viewport_size(ViewportId viewport_id, const Math::Vector2U &framebuffer_size)
{
    const Math::Vector2 &viewport_size{s_viewports[to_index(viewport_id)].rect.size};
    return Math::Vector2I{Math::round(Math::scale(viewport_size, Math::Vector2{framebuffer_size}))};
}

MappedViewport mapped_viewport(ViewportId viewport_id, const Math::Vector2U &framebuffer_size)
{
    const Viewport &viewport{s_viewports[to_index(viewport_id)]};
    return {
        .rect{Math::round(Math::scale(viewport.rect, Math::Vector2{framebuffer_size}))},
        .near_depth{viewport.near_depth},
        .far_depth{viewport.far_depth}
    };
}
} // namespace Age::Gfx
