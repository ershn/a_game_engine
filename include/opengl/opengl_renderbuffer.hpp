#pragma once

#include <cstdint>
#include <type_traits>

#include "image_formats.hpp"
#include "opengl/opengl_api.hpp"
#include "render_target.hpp"

namespace Age::Gfx::OGL
{
inline namespace Api
{
struct RenderbufferDesc
{
    std::uint32_t width{};
    std::uint32_t height{};
    ImageFormat format{};
};

enum struct RenderbufferId : std::underlying_type_t<RenderTargetId>
{
};
} // namespace Api

struct Renderbuffer
{
    GLuint gl_object{};
};

inline namespace Api
{
void init_renderbuffer_system();

bool is_renderbuffer_id(RenderTargetId id);
RenderTargetId to_render_target_id(RenderbufferId id);
RenderbufferId to_renderbuffer_id(RenderTargetId id);

RenderbufferId create_renderbuffer(const RenderbufferDesc &renderbuffer_desc);

const RenderbufferDesc &get_renderbuffer_desc(RenderbufferId id);

void resize_renderbuffer(RenderbufferId id, std::uint32_t width, std::uint32_t height);
} // namespace Api

const Renderbuffer &get_renderbuffer(RenderbufferId id);
} // namespace Age::Gfx::OGL
