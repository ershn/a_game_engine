#include <algorithm>
#include <bit>
#include <limits>
#include <vector>

#include "error_handling.hpp"
#include "id_generator.hpp"
#include "opengl/opengl_api.hpp"
#include "opengl/opengl_errors.hpp"
#include "opengl/opengl_framebuffer.hpp"
#include "opengl/opengl_image_formats.hpp"
#include "opengl/opengl_renderbuffer.hpp"

namespace Age::Gfx::OGL
{
namespace
{
constexpr GLenum s_blit_filter_to_gl_enum[] = {GL_NEAREST, GL_LINEAR};

constexpr GLenum to_gl_enum(BlitFilter blit_filter)
{
    return s_blit_filter_to_gl_enum[static_cast<std::size_t>(blit_filter)];
}

struct Framebuffer
{
    GLuint gl_object{};
};

int s_framebuffer_real_width{};
int s_framebuffer_real_height{};
bool s_framebuffer_real_size_changed{};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    s_framebuffer_real_width = width;
    s_framebuffer_real_height = height;

    s_framebuffer_real_size_changed = true;
}

Util::IdGenerator<FramebufferId> s_framebuffer_id_generator{FramebufferId{2}};
std::vector<Framebuffer> s_framebuffers{Framebuffer{.gl_object = 0}};
std::vector<FramebufferAttachments> s_framebuffer_attachments{{}};
std::vector<FramebufferMask> s_framebuffer_masks{{.color_buffers = 0b1, .depth_buffer = true, .stencil_buffer = true}};
std::vector<Math::Vector2U> s_framebuffer_sizes{{}};

bool s_system_framebuffer_size_changed{};

FramebufferId s_bound_read_framebuffer_id{SYSTEM_FRAMEBUFFER_ID};
FramebufferId s_bound_draw_framebuffer_id{SYSTEM_FRAMEBUFFER_ID};

constexpr std::size_t to_index(FramebufferId id)
{
    return static_cast<std::size_t>(id) - 1;
}

void set_read_framebuffer(FramebufferId framebuffer_id)
{
    if (framebuffer_id != s_bound_read_framebuffer_id)
    {
        const Framebuffer &framebuffer{s_framebuffers[to_index(framebuffer_id)]};
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.gl_object);
        s_bound_read_framebuffer_id = framebuffer_id;
    }
}

void set_draw_framebuffer(FramebufferId framebuffer_id)
{
    if (framebuffer_id != s_bound_draw_framebuffer_id)
    {
        const Framebuffer &framebuffer{s_framebuffers[to_index(framebuffer_id)]};
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.gl_object);
        s_bound_draw_framebuffer_id = framebuffer_id;
    }
}

void set_read_color_buffer(std::uint8_t color_buffer)
{
    if (color_buffer == 0)
    {
        glReadBuffer(GL_NONE);
        return;
    }

    if (s_bound_read_framebuffer_id == SYSTEM_FRAMEBUFFER_ID)
    {
        BAIL_ERROR_IF(
            color_buffer != 0b1,
            "a non-existent color buffer was specified for the system framebuffer: {:b}",
            color_buffer
        );
        glReadBuffer(GL_BACK_LEFT);
    }
    else
    {
        int read_buffer_index{std::countr_zero(color_buffer)};
        glReadBuffer(GL_COLOR_ATTACHMENT0 + read_buffer_index);
    }
}

void set_draw_color_buffers(std::uint8_t color_buffers)
{
    if (s_bound_draw_framebuffer_id == SYSTEM_FRAMEBUFFER_ID)
    {
        if (color_buffers == 0)
        {
            glDrawBuffer(GL_NONE);
            return;
        }

        BAIL_ERROR_IF(
            color_buffers != 0b1,
            "a non-existent color buffer was specified for the system framebuffer: {:b}",
            color_buffers
        );
        glDrawBuffer(GL_BACK_LEFT);
    }
    else
    {
        GLenum draw_buffers[COLOR_ATTACHMENT_POINT_COUNT];
        for (std::size_t index{}; index < COLOR_ATTACHMENT_POINT_COUNT; ++index)
        {
            draw_buffers[index] =
                color_buffers & 1 << index ? static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + index) : GL_NONE;
        }
        glDrawBuffers(COLOR_ATTACHMENT_POINT_COUNT, draw_buffers);
    }
}

Math::Vector2U attach_image(GLenum framebuffer_target, GLenum attachment_point, const FramebufferAttachment &attachment)
{
    if (is_texture_id(attachment.render_target_id))
    {
        return {};
    }
    else
    {
        RenderbufferId renderbuffer_id{to_renderbuffer_id(attachment.render_target_id)};
        const Renderbuffer &renderbuffer{get_renderbuffer(renderbuffer_id)};
        const RenderbufferDesc &renderbuffer_desc{get_renderbuffer_desc(renderbuffer_id)};

        glFramebufferRenderbuffer(framebuffer_target, attachment_point, GL_RENDERBUFFER, renderbuffer.gl_object);

        return {renderbuffer_desc.width, renderbuffer_desc.height};
    }
}
} // namespace

namespace Api
{
void init_framebuffer_system(GLFWwindow *window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Math::Vector2U &system_framebuffer_size{s_framebuffer_sizes[to_index(SYSTEM_FRAMEBUFFER_ID)]};
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    system_framebuffer_size.x = static_cast<unsigned int>(width);
    system_framebuffer_size.y = static_cast<unsigned int>(height);

    s_system_framebuffer_size_changed = true;
}

FramebufferId create_framebuffer(const FramebufferAttachments &attachments)
{
    FramebufferId framebuffer_id{s_framebuffer_id_generator.generate()};
    std::size_t framebuffer_index{to_index(framebuffer_id)};

    if (framebuffer_index == s_framebuffers.size())
    {
        s_framebuffers.resize(framebuffer_index + 1);
        s_framebuffer_attachments.resize(framebuffer_index + 1);
        s_framebuffer_masks.resize(framebuffer_index + 1);
        s_framebuffer_sizes.resize(framebuffer_index + 1);
    }

    s_framebuffer_attachments[framebuffer_index] = attachments;

    Framebuffer &framebuffer{s_framebuffers[framebuffer_index]};
    FramebufferMask &framebuffer_mask{s_framebuffer_masks[framebuffer_index]};

    Math::Vector2U min_attachment_size{Math::max<Math::Vector2U>()};

    glGenFramebuffers(1, &framebuffer.gl_object);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.gl_object);

    for (std::size_t index{}; index < COLOR_ATTACHMENT_POINT_COUNT; ++index)
    {
        const FramebufferAttachment &attachment{attachments.color_attachments[index]};

        if (attachment.render_target_id != RenderTargetId{})
        {
            auto attachment_size =
                attach_image(GL_READ_FRAMEBUFFER, static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + index), attachment);

            min_attachment_size = Math::min(min_attachment_size, attachment_size);
            framebuffer_mask.color_buffers |= 1U << index;
        }
    }

    if (attachments.depth_attachment.render_target_id != RenderTargetId{})
    {
        auto attachment_size = attach_image(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attachments.depth_attachment);

        min_attachment_size = Math::min(min_attachment_size, attachment_size);
        framebuffer_mask.depth_buffer = true;
    }

    if (attachments.stencil_attachment.render_target_id != RenderTargetId{})
    {
        auto attachment_size = attach_image(GL_READ_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, attachments.stencil_attachment);

        min_attachment_size = Math::min(min_attachment_size, attachment_size);
        framebuffer_mask.stencil_buffer = true;
    }

    const Framebuffer &bound_framebuffer{s_framebuffers[to_index(s_bound_read_framebuffer_id)]};
    glBindFramebuffer(GL_READ_FRAMEBUFFER, bound_framebuffer.gl_object);

    if (min_attachment_size != Math::max<Math::Vector2U>())
        s_framebuffer_sizes[framebuffer_index] = min_attachment_size;

    return framebuffer_id;
}

bool has_system_framebuffer_size_changed()
{
    return s_system_framebuffer_size_changed;
}

const Math::Vector2U &get_framebuffer_size(FramebufferId framebuffer_id)
{
    return s_framebuffer_sizes[to_index(framebuffer_id)];
}

void update_system_framebuffer_size()
{
    if (s_framebuffer_real_size_changed)
    {
        s_framebuffer_real_size_changed = false;

        Math::Vector2U &framebuffer_size{s_framebuffer_sizes[to_index(SYSTEM_FRAMEBUFFER_ID)]};
        framebuffer_size.x = static_cast<unsigned int>(s_framebuffer_real_width);
        framebuffer_size.y = static_cast<unsigned int>(s_framebuffer_real_height);

        s_system_framebuffer_size_changed = true;
    }
    else
    {
        s_system_framebuffer_size_changed = false;
    }
}

void update_user_framebuffer_size(FramebufferId framebuffer_id, const Math::Vector2U &size)
{
    s_framebuffer_sizes[to_index(framebuffer_id)] = size;
}

void set_render_targets(FramebufferId framebuffer_id, std::uint8_t color_buffers)
{
    set_draw_framebuffer(framebuffer_id);
    set_draw_color_buffers(color_buffers);
}

void clear_framebuffer(FramebufferId framebuffer_id, const FramebufferClear &clear)
{
    std::size_t framebuffer_index{to_index(framebuffer_id)};
    FramebufferMask framebuffer_mask{FramebufferMask::from(clear.buffers & s_framebuffer_masks[framebuffer_index])};

    set_draw_framebuffer(framebuffer_id);

    for (std::size_t index{}; 1 << index <= framebuffer_mask.color_buffers; ++index)
    {
        if (framebuffer_mask.color_buffers & 1 << index)
            glClearBufferfv(GL_COLOR, static_cast<GLint>(index), static_cast<const float *>(clear.clear_colors[index]));
    }

    if (framebuffer_mask.depth_buffer && framebuffer_mask.stencil_buffer)
        glClearBufferfi(GL_DEPTH_STENCIL, 0, clear.clear_depth, clear.clear_stencil_index);
    else if (framebuffer_mask.depth_buffer)
        glClearBufferfv(GL_DEPTH, 0, &clear.clear_depth);
    else if (framebuffer_mask.stencil_buffer)
        glClearBufferiv(GL_STENCIL, 0, &clear.clear_stencil_index);
}

void blit_framebuffer(
    FramebufferId source_id,
    const Math::Rectangle &source_rect,
    FramebufferId dest_id,
    const Math::Rectangle &dest_rect,
    std::uint8_t source_color_buffer,
    std::uint8_t dest_color_buffers,
    bool blit_depth_buffer,
    bool blit_stencil_buffer,
    BlitFilter filter
)
{
    set_read_framebuffer(source_id);
    set_draw_framebuffer(dest_id);

    if (source_color_buffer && dest_color_buffers)
    {
        set_read_color_buffer(source_color_buffer);
        set_draw_color_buffers(dest_color_buffers);
    }

    GLbitfield buffer_mask{};
    if (source_color_buffer && dest_color_buffers)
        buffer_mask |= GL_COLOR_BUFFER_BIT;
    if (blit_depth_buffer)
        buffer_mask |= GL_DEPTH_BUFFER_BIT;
    if (blit_stencil_buffer)
        buffer_mask |= GL_STENCIL_BUFFER_BIT;

    OGL_CLEAR_ERRORS();

    Math::RectangleI source_rect_i{Math::scale(source_rect, get_framebuffer_size(source_id))};
    Math::RectangleI dest_rect_i{Math::scale(dest_rect, get_framebuffer_size(dest_id))};

    glBlitFramebuffer(
        source_rect_i.position.x,
        source_rect_i.position.y,
        source_rect_i.position.x + source_rect_i.size.x,
        source_rect_i.position.y + source_rect_i.size.y,
        dest_rect_i.position.x,
        dest_rect_i.position.y,
        dest_rect_i.position.x + dest_rect_i.size.x,
        dest_rect_i.position.y + dest_rect_i.size.y,
        buffer_mask,
        to_gl_enum(filter)
    );

    OGL_LOG_ERROR();
}

TextureId create_texture_from_framebuffer(FramebufferId framebuffer_id, ImageFormat image_format)
{
    return create_texture(TextureType::TEXTURE_2D, [&](const Texture &, TextureDesc &texture_desc) {
        const Math::Vector2U &framebuffer_size{get_framebuffer_size(framebuffer_id)};

        set_read_framebuffer(framebuffer_id);

        GLint internal_format{get_internal_format(image_format, false)};
        glCopyTexImage2D(
            GL_TEXTURE_2D,
            0,
            static_cast<GLenum>(internal_format),
            0,
            0,
            static_cast<GLsizei>(framebuffer_size.x),
            static_cast<GLsizei>(framebuffer_size.y),
            0
        );

        texture_desc = {
            .width = framebuffer_size.x,
            .height = framebuffer_size.y,
            .depth = 1,
            .mipmap_level_count = 1,
            .count = 1,
            .format = image_format,
            .type = TextureType::TEXTURE_2D
        };
    });
}

void copy_framebuffer_to_texture(FramebufferId framebuffer_id, TextureId texture_id)
{
    modify_texture(texture_id, [&](const Texture &, TextureDesc &texture_desc) {
        const Math::Vector2U &framebuffer_size{get_framebuffer_size(framebuffer_id)};

        set_read_framebuffer(framebuffer_id);

        if (framebuffer_size.x == texture_desc.width && framebuffer_size.y == texture_desc.height)
        {
            glCopyTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                0,
                0,
                0,
                static_cast<GLsizei>(framebuffer_size.x),
                static_cast<GLsizei>(framebuffer_size.y)
            );
        }
        else
        {
            GLint internal_format{get_internal_format(texture_desc.format, false)};
            glCopyTexImage2D(
                GL_TEXTURE_2D,
                0,
                static_cast<GLenum>(internal_format),
                0,
                0,
                static_cast<GLsizei>(framebuffer_size.x),
                static_cast<GLsizei>(framebuffer_size.y),
                0
            );

            texture_desc.width = framebuffer_size.x;
            texture_desc.height = framebuffer_size.y;
        }
    });
}
} // namespace Api
} // namespace Age::Gfx::OGL
