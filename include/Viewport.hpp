#pragma once

#include <cstdint>

#include "Framebuffer.hpp"
#include "Rectangle.hpp"

namespace Age::Gfx
{
enum struct ViewportId : std::uint16_t
{
};

inline constexpr ViewportId FULL_VIEWPORT_ID{1};

struct Viewport
{
    Rectangle norm_rect{};
};

void init_viewport_system();

ViewportId create_viewport(const Rectangle &norm_rect);

Viewport &get_viewport(ViewportId viewport_id);

RectangleI calc_viewport_pixel_rect(const Viewport &viewport, const Framebuffer &framebuffer);

void use_viewport_pixel_rect(const RectangleI &pixel_rect);
} // namespace Age::Gfx
