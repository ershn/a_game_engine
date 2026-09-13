#pragma once

#include <array>
#include <cstdint>
#include <limits>

#include "glfw.hpp"
#include "opengl/opengl_texture.hpp"
#include "rectangle.hpp"
#include "render_target.hpp"

namespace Age::Gfx::OGL
{
inline namespace Api
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
} // namespace Api

struct Framebuffer
{
    GLuint gl_object{};
};

inline namespace Api
{
void init_framebuffer_system(GLFWwindow *window);

FramebufferId create_framebuffer(const FramebufferAttachments &attachments);

bool has_system_framebuffer_size_changed();

const Math::Vector2U &get_framebuffer_size(FramebufferId framebuffer_id);

void update_system_framebuffer_size();
void update_user_framebuffer_size(FramebufferId framebuffer_id, const Math::Vector2U &size);

void set_render_targets(FramebufferId framebuffer_id, ColorBufferMask color_buffers);

struct FramebufferClear
{
    FramebufferMask buffers{
        .color_buffers = std::numeric_limits<ColorBufferMask>::max(), .depth_buffer = true, .stencil_buffer = true
    };
    std::array<Math::Vector4, COLOR_ATTACHMENT_POINT_COUNT> clear_colors{};
    float clear_depth{1.0f};
    std::int32_t clear_stencil_index{};
};

void clear_framebuffer(FramebufferId framebuffer_id, const FramebufferClear &clear);
void clear_framebuffer(FramebufferId framebuffer_id, const FramebufferClear &clear, const Math::RectangleI &rect);

enum struct BlitFilter : std::uint8_t
{
    NEAREST,
    LINEAR
};

void blit_framebuffer(
    FramebufferId source_id,
    const Math::Rectangle &source_rect,
    FramebufferId dest_id,
    const Math::Rectangle &dest_rect,
    ColorBufferMask source_color_buffer,
    ColorBufferMask dest_color_buffers,
    bool blit_depth_buffer,
    bool blit_stencil_buffer,
    BlitFilter filter
);

TextureId create_texture_from_framebuffer(FramebufferId framebuffer_id, ImageFormat image_format);
void copy_framebuffer_to_texture(FramebufferId framebuffer_id, TextureId texture_id);
} // namespace Api
} // namespace Age::Gfx::OGL
