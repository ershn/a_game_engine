#include <limits>

#include "id_generator.hpp"
#include "opengl/opengl_image_formats.hpp"
#include "opengl/opengl_renderbuffer.hpp"
#include "utils.hpp"

namespace Age::Gfx::OGL
{
namespace
{
constexpr auto MAX_RENDER_TARGET_ID{std::numeric_limits<std::underlying_type_t<RenderTargetId>>::max()};
constexpr RenderbufferId FIRST_RENDERBUFFER_ID{MAX_RENDER_TARGET_ID / 2 + 1};
constexpr RenderbufferId LAST_RENDERBUFFER_ID{MAX_RENDER_TARGET_ID};

constexpr std::size_t to_index(RenderbufferId id)
{
    return static_cast<std::size_t>(id) - static_cast<std::size_t>(FIRST_RENDERBUFFER_ID);
}

Util::IdGenerator<RenderbufferId> s_renderbuffer_id_generator{FIRST_RENDERBUFFER_ID};
std::vector<Renderbuffer> s_renderbuffers;
std::vector<RenderbufferDesc> s_renderbuffer_descs;
} // namespace

namespace Api
{
void init_renderbuffer_system()
{
    s_renderbuffers.reserve(8);
    s_renderbuffer_descs.reserve(8);
}

bool is_renderbuffer_id(RenderTargetId id)
{
    return Util::to_underlying(FIRST_RENDERBUFFER_ID) <= Util::to_underlying(id) &&
           Util::to_underlying(id) <= Util::to_underlying(LAST_RENDERBUFFER_ID);
}

RenderTargetId to_render_target_id(RenderbufferId id)
{
    return static_cast<RenderTargetId>(id);
}

RenderbufferId to_renderbuffer_id(RenderTargetId id)
{
    return static_cast<RenderbufferId>(id);
}

RenderbufferId create_renderbuffer(const RenderbufferDesc &renderbuffer_desc)
{
    RenderbufferId renderbuffer_id{s_renderbuffer_id_generator.generate()};
    std::size_t renderbuffer_index{to_index(renderbuffer_id)};

    if (renderbuffer_index == s_renderbuffers.size())
    {
        s_renderbuffers.resize(renderbuffer_index + 1);
        s_renderbuffer_descs.resize(renderbuffer_index + 1);
    }

    s_renderbuffer_descs[renderbuffer_index] = renderbuffer_desc;

    Renderbuffer &renderbuffer{s_renderbuffers[renderbuffer_index]};

    glGenRenderbuffers(1, &renderbuffer.gl_object);

    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer.gl_object);

    GLint internal_format{get_internal_format(renderbuffer_desc.format, false)};
    glRenderbufferStorage(GL_RENDERBUFFER, internal_format, renderbuffer_desc.width, renderbuffer_desc.height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return renderbuffer_id;
}

const RenderbufferDesc &get_renderbuffer_desc(RenderbufferId id)
{
    return s_renderbuffer_descs[to_index(id)];
}

void resize_renderbuffer(RenderbufferId id, std::uint32_t width, std::uint32_t height)
{
    std::size_t renderbuffer_index{to_index(id)};

    RenderbufferDesc &renderbuffer_desc{s_renderbuffer_descs[renderbuffer_index]};
    renderbuffer_desc.width = width;
    renderbuffer_desc.height = height;

    const Renderbuffer &renderbuffer{s_renderbuffers[renderbuffer_index]};

    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer.gl_object);

    GLint internal_format{get_internal_format(renderbuffer_desc.format, false)};
    glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
} // namespace Api

const Renderbuffer &get_renderbuffer(RenderbufferId id)
{
    return s_renderbuffers[to_index(id)];
}
} // namespace Age::Gfx::OGL
