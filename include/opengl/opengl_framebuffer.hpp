#pragma once

#include "common_framebuffer.hpp"
#include "glfw.hpp"
#include "opengl/opengl_texture.hpp"
#include "rectangle.hpp"

namespace Age::Gfx::OGL
{
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

void clear_framebuffer(FramebufferId framebuffer_id, const FramebufferClear &clear);
void clear_framebuffer(FramebufferId framebuffer_id, const FramebufferClear &clear, const Math::RectangleI &rect);

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
