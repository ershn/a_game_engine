#pragma once

#include <cstdint>

#include "rectangle.hpp"

namespace Age::Gfx
{
enum struct ViewportId : std::uint16_t
{
};

inline constexpr ViewportId FULL_VIEWPORT_ID{1};

struct Viewport
{
    Math::Rectangle norm_rect{};
};

void init_viewport_system();

ViewportId create_viewport(const Math::Rectangle &norm_rect);

Viewport &get_viewport(ViewportId viewport_id);

Math::RectangleI calc_viewport_pixel_rect(const Viewport &viewport, const Math::Vector2U &framebuffer_size);

void use_viewport_pixel_rect(const Math::RectangleI &pixel_rect);
} // namespace Age::Gfx
