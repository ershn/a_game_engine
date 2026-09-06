#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <type_traits>

#include "image_formats.hpp"
#include "opengl/opengl_api.hpp"
#include "render_target.hpp"

namespace Age::Gfx
{
float get_texture_filtering_max_max_anisotropy();

enum struct TextureType : std::uint8_t
{
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_CUBE_MAP,
    TEXTURE_3D
};

enum struct AlphaType : std::uint8_t
{
    UNKNOWN,
    STRAIGHT,
    PREMULTIPLIED
};

struct TextureDesc
{
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t depth{};
    std::uint32_t mipmap_level_count{};
    std::uint32_t count{};
    ImageFormat format{};
    TextureType type{};
    AlphaType alpha_type{};
};

struct TextureData
{
    std::unique_ptr<std::byte[]> bytes{};
    TextureDesc desc{};
};

struct CubeMapFace;

struct CubeMap
{
    const TextureData &texture;
    std::uint32_t face_size{};

    CubeMapFace operator[](std::size_t index) const;
};

struct CubeMapFace
{
    const CubeMap &cube_map;
    std::uint32_t byte_offset{};
    std::uint32_t face_index{};
};

CubeMap get_cube_map(const TextureData &texture);

bool operator==(const CubeMapFace &it1, const CubeMapFace &it2);
bool operator!=(const CubeMapFace &it1, const CubeMapFace &it2);
CubeMapFace &operator++(CubeMapFace &it);
const CubeMapFace &operator*(const CubeMapFace &it);

CubeMapFace begin(const CubeMap &cube_map);
CubeMapFace end(const CubeMap &cube_map);

struct Mipmap
{
    const TextureData &texture;
    std::uint32_t byte_offset{};
};

struct MipmapLevel
{
    const TextureData &texture;
    std::uint32_t byte_offset{};
    std::uint32_t level{};
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t depth{};
    std::uint32_t pitch{};
    std::uint32_t row_count{};
};

Mipmap get_mipmap(const TextureData &texture);
Mipmap get_mipmap(const CubeMapFace &cube_map_face);

bool operator==(const MipmapLevel &it1, const MipmapLevel &it2);
bool operator!=(const MipmapLevel &it1, const MipmapLevel &it2);
MipmapLevel &operator++(MipmapLevel &it);
const MipmapLevel &operator*(const MipmapLevel &it);

MipmapLevel begin(const Mipmap &mipmap);
MipmapLevel end(const Mipmap &mipmap);

using TextureUnitId = std::uint16_t;

inline constexpr TextureUnitId NULL_TEXTURE_UNIT_ID{std::numeric_limits<TextureUnitId>::max()};

enum struct TextureId : std::underlying_type_t<RenderTargetId>
{
};

struct Texture;

enum struct SamplerId : std::uint16_t
{
};

struct Sampler;

struct SamplerUniform
{
    GLint uniform{-1};
    TextureUnitId bound_texture_unit_id{NULL_TEXTURE_UNIT_ID};
};

enum struct TextureWrapMode : std::uint8_t
{
    REPEAT,
    MIRRORED_REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER
};

enum struct TextureMagFilter : std::uint8_t
{
    NEAREST,
    LINEAR
};

enum struct TextureMinFilter : std::uint8_t
{
    NEAREST,
    LINEAR,
    NEAREST_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_NEAREST,
    LINEAR_MIPMAP_LINEAR
};

struct SamplerFlags
{
    TextureWrapMode texture_wrap_s : 2 {};
    TextureWrapMode texture_wrap_t : 2 {};
    TextureWrapMode texture_wrap_r : 2 {};
    TextureMagFilter texture_mag_filter : 1 {};
    TextureMinFilter texture_min_filter : 3 {};
};

struct SamplerParams
{
    float max_anisotropy{1.0f};
    Math::Vector4 texture_border_color{0.0f};
    SamplerFlags flags{};
};

struct TextureCreationOptions
{
    bool force_srgb_internal_format : 1 {};
};

void init_texture_system();

bool is_texture_id(RenderTargetId id);
RenderTargetId to_render_target_id(TextureId id);
TextureId to_texture_id(RenderTargetId id);

TextureId create_texture(const TextureData &texture_data, TextureCreationOptions creation_options = {});
TextureId create_texture(TextureType texture_type, std::function<void(const Texture &, TextureDesc &)> specify_texture);

void modify_texture(TextureId texture_id, std::function<void(const Texture &, TextureDesc &)> modify_texture);

SamplerId create_sampler(const SamplerParams &sampler_params);
const SamplerParams &get_sampler_params(SamplerId sampler_id);
void set_sampler_params(SamplerId sampler_id, const SamplerParams &sampler_params);

// The shader program corresponding to the uniform needs to be in use before the function call.
void bind_texture_and_sampler(SamplerUniform &sampler_uniform, TextureId texture_id, SamplerId sampler_id);
} // namespace Age::Gfx
