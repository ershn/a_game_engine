#include "Color.hpp"

namespace Age::Gfx
{
Math::Vector4 gamma_correct_color(const Math::Vector4 &color, float gamma_reciprocal)
{
    Math::Vector4 exponents{gamma_reciprocal, gamma_reciprocal, gamma_reciprocal, 1.0f};
    return Math::pow(color, exponents);
}
} // namespace Age::Gfx
