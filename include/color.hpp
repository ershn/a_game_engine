#pragma once

#include "vector.hpp"

namespace Age::Gfx
{
Math::Vector4 gamma_correct_color(const Math::Vector4 &color, float gamma_reciprocal);
} // namespace Age::Gfx
