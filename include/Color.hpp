#pragma once

#include "Vector.hpp"

namespace Age::Gfx
{
Math::Vector4 gamma_correct_color(const Math::Vector4 &color, float gamma_inverse);
} // namespace Age::Gfx
