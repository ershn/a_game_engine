#pragma once

#include <cstdint>
#include <memory>

#include "image_formats.hpp"
#include "vector.hpp"

namespace Age::Gfx
{
// Textures

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

std::uint32_t get_base_image_pixel_data_size(const TextureDesc &texture_desc);

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

std::uint32_t get_mipmap_level_pixel_data_size(const MipmapLevel &mipmap_level);
std::uint32_t get_mipmap_pixel_data_size(const TextureData &texture);

struct TextureCreationOptions
{
    bool force_srgb_internal_format : 1 {};
};

// Sampling

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
} // namespace Age::Gfx
