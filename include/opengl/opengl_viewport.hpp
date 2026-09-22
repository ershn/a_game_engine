#pragma once

#include "common_viewport.hpp"

namespace Age::Gfx::OGL
{
inline namespace Api
{
void init_viewport_system();

void set_viewport_transformation(const MappedViewport &mapped_viewport);
} // namespace Api
} // namespace Age::Gfx::OGL
