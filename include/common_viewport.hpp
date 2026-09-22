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
    Math::Rectangle rect{{0.0f, 0.0f}, {1.0f, 1.0f}};
    float near_depth{0.0f};
    float far_depth{1.0f};
};

struct MappedViewport
{
    Math::RectangleI rect{};
    float near_depth{};
    float far_depth{};
};

void init_viewport_storage();

ViewportId create_viewport(const Viewport &viewport);

Math::Vector2I mapped_viewport_size(ViewportId viewport_id, const Math::Vector2U &framebuffer_size);
MappedViewport mapped_viewport(ViewportId viewport_id, const Math::Vector2U &framebuffer_size);
} // namespace Age::Gfx
