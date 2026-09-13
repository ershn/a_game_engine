#pragma once

#include "vector.hpp"

namespace Age::Gfx
{
Math::Vector4 linear_to_srgb(const Math::Vector4 &color, float gamma);
Math::Vector4 srgb_to_linear(const Math::Vector4 &color, float gamma);
} // namespace Age::Gfx
