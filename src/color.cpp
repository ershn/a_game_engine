#include "color.hpp"

namespace Age::Gfx
{
Math::Vector4 linear_to_srgb(const Math::Vector4 &color, float gamma)
{
    float gamma_reciprocal{1.0f / gamma};
    Math::Vector4 exponents{gamma_reciprocal, gamma_reciprocal, gamma_reciprocal, 1.0f};
    return Math::pow(color, exponents);
}

Math::Vector4 srgb_to_linear(const Math::Vector4 &color, float gamma)
{
    Math::Vector4 exponents{gamma, gamma, gamma, 1.0f};
    return Math::pow(color, exponents);
}
} // namespace Age::Gfx
