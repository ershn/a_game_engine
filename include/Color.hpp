#pragma once

#include "Components.hpp"
#include "UniformBlocks.hpp"
#include "UniformBuffer.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
struct GlobalColorSettings
{
    static constexpr auto TYPE{Core::ComponentType::GLOBAL_COLOR_SETTINGS};

    float gamma_inverse{};
};

struct GammaCorrectionBufferBlockRef : public UniformBufferBlockRef<GammaCorrectionBlock>
{
    static constexpr auto TYPE{Core::ComponentType::GAMMA_CORRECTION_BUFFER_BLOCK};

    using UniformBufferBlockRef<GammaCorrectionBlock>::operator=;
};

Math::Vector4 gamma_correct_color(const Math::Vector4 &color, float gamma_inverse);
} // namespace Age::Gfx
