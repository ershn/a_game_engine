#pragma once

#include <array>
#include <cstdint>
#include <limits>

#include "render_target.hpp"
#include "vector.hpp"

namespace Age::Gfx
{
enum struct FramebufferId : std::uint16_t
{
};

inline constexpr FramebufferId SYSTEM_FRAMEBUFFER_ID{1};

struct FramebufferAttachment
{
    RenderTargetId render_target_id{};
    std::uint32_t texture_mipmap_level{};
    std::uint32_t texture_layer{};
};

inline constexpr std::size_t COLOR_ATTACHMENT_POINT_COUNT{8};

struct FramebufferAttachments
{
    std::array<FramebufferAttachment, COLOR_ATTACHMENT_POINT_COUNT> color_attachments{};
    FramebufferAttachment depth_attachment{};
    FramebufferAttachment stencil_attachment{};
};

using ColorBufferMask = std::uint8_t;

struct FramebufferMask
{
    ColorBufferMask color_buffers{};
    bool depth_buffer : 1 {};
    bool stencil_buffer : 1 {};

    static constexpr FramebufferMask from(unsigned int mask)
    {
        return {
            .color_buffers = static_cast<ColorBufferMask>(mask >> 8),
            .depth_buffer = static_cast<bool>(mask & 0b10),
            .stencil_buffer = static_cast<bool>(mask & 0b1)
        };
    }

    constexpr operator unsigned int() const
    {
        return color_buffers << 8 | depth_buffer << 1 | static_cast<int>(stencil_buffer);
    }
};

struct FramebufferClear
{
    FramebufferMask buffers{
        .color_buffers = std::numeric_limits<ColorBufferMask>::max(), .depth_buffer = true, .stencil_buffer = true
    };
    std::array<Math::Vector4, COLOR_ATTACHMENT_POINT_COUNT> clear_colors{};
    float clear_depth{1.0f};
    std::int32_t clear_stencil_index{};
};

enum struct BlitFilter : std::uint8_t
{
    NEAREST,
    LINEAR
};
} // namespace Age::Gfx
