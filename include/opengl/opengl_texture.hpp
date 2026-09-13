#pragma once

#include <functional>

#include "common_texture.hpp"
#include "opengl/opengl_api.hpp"
#include "render_target.hpp"

namespace Age::Gfx::OGL
{
inline namespace Api
{
float get_texture_filtering_max_max_anisotropy();

enum struct TextureId : std::underlying_type_t<RenderTargetId>
{
};

enum struct SamplerId : std::uint16_t
{
};

using TextureUnitId = std::uint16_t;

inline constexpr TextureUnitId NULL_TEXTURE_UNIT_ID{std::numeric_limits<TextureUnitId>::max()};

struct SamplerUniform
{
    GLint uniform{-1};
    TextureUnitId bound_texture_unit_id{NULL_TEXTURE_UNIT_ID};
};
} // namespace Api

struct Texture
{
    GLuint gl_object{};
    TextureUnitId bound_texture_unit_id{NULL_TEXTURE_UNIT_ID};
    TextureType type{};
};

struct Sampler
{
    GLuint gl_object{};
    TextureUnitId bound_texture_unit_id{NULL_TEXTURE_UNIT_ID};
};

struct TextureUnit
{
    TextureId bound_texture_id{};
    std::uint32_t sampler_use_count{};
    SamplerId bound_sampler_id{};
};

inline namespace Api
{
void init_texture_system();

bool is_texture_id(RenderTargetId id);
RenderTargetId to_render_target_id(TextureId id);
TextureId to_texture_id(RenderTargetId id);

TextureId create_texture(const TextureData &texture_data, TextureCreationOptions creation_options = {});

SamplerId create_sampler(const SamplerParams &sampler_params);
const SamplerParams &get_sampler_params(SamplerId sampler_id);
void set_sampler_params(SamplerId sampler_id, const SamplerParams &sampler_params);

// The shader program corresponding to the uniform needs to be in use before the function call.
void bind_texture_and_sampler(SamplerUniform &sampler_uniform, TextureId texture_id, SamplerId sampler_id);
} // namespace Api

TextureId create_texture(TextureType texture_type, std::function<void(const Texture &, TextureDesc &)> specify_texture);

void modify_texture(TextureId texture_id, std::function<void(const Texture &, TextureDesc &)> modify_texture);
} // namespace Age::Gfx::OGL
