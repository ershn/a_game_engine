#include "Color.hpp"

namespace Age::Gfx
{
Math::Vector4 gamma_correct_color(const Math::Vector4 &color, float gamma_inverse)
{
    Math::Vector4 exponents{gamma_inverse, gamma_inverse, gamma_inverse, 1.0f};
    return Math::pow(color, exponents);
}
} // namespace Age::Gfx
