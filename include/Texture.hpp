#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <span>

#include "OpenGL.hpp"

namespace Age::Gfx
{
float get_texture_filtering_max_max_anisotropy();

enum struct TextureType : std::uint8_t
{
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D
};

enum struct TextureFormat : std::uint32_t
{
    UNKNOWN = 0,

    // DXGI_FORMAT formats
    FIRST_DXGI_FORMAT = 1,
    R32G32B32A32_TYPELESS = FIRST_DXGI_FORMAT,
    R32G32B32A32_FLOAT = 2,
    R32G32B32A32_UINT = 3,
    R32G32B32A32_SINT = 4,
    R32G32B32_TYPELESS = 5,
    R32G32B32_FLOAT = 6,
    R32G32B32_UINT = 7,
    R32G32B32_SINT = 8,
    R16G16B16A16_TYPELESS = 9,
    R16G16B16A16_FLOAT = 10,
    R16G16B16A16_UNORM = 11,
    R16G16B16A16_UINT = 12,
    R16G16B16A16_SNORM = 13,
    R16G16B16A16_SINT = 14,
    R32G32_TYPELESS = 15,
    R32G32_FLOAT = 16,
    R32G32_UINT = 17,
    R32G32_SINT = 18,
    R32G8X24_TYPELESS = 19,
    D32_FLOAT_S8X24_UINT = 20,
    R32_FLOAT_X8X24_TYPELESS = 21,
    X32_TYPELESS_G8X24_UINT = 22,
    R10G10B10A2_TYPELESS = 23,
    R10G10B10A2_UNORM = 24,
    R10G10B10A2_UINT = 25,
    R11G11B10_FLOAT = 26,
    R8G8B8A8_TYPELESS = 27,
    R8G8B8A8_UNORM = 28,
    R8G8B8A8_UNORM_SRGB = 29,
    R8G8B8A8_UINT = 30,
    R8G8B8A8_SNORM = 31,
    R8G8B8A8_SINT = 32,
    R16G16_TYPELESS = 33,
    R16G16_FLOAT = 34,
    R16G16_UNORM = 35,
    R16G16_UINT = 36,
    R16G16_SNORM = 37,
    R16G16_SINT = 38,
    R32_TYPELESS = 39,
    D32_FLOAT = 40,
    R32_FLOAT = 41,
    R32_UINT = 42,
    R32_SINT = 43,
    R24G8_TYPELESS = 44,
    D24_UNORM_S8_UINT = 45,
    R24_UNORM_X8_TYPELESS = 46,
    X24_TYPELESS_G8_UINT = 47,
    R8G8_TYPELESS = 48,
    R8G8_UNORM = 49,
    R8G8_UINT = 50,
    R8G8_SNORM = 51,
    R8G8_SINT = 52,
    R16_TYPELESS = 53,
    R16_FLOAT = 54,
    D16_UNORM = 55,
    R16_UNORM = 56,
    R16_UINT = 57,
    R16_SNORM = 58,
    R16_SINT = 59,
    R8_TYPELESS = 60,
    R8_UNORM = 61,
    R8_UINT = 62,
    R8_SNORM = 63,
    R8_SINT = 64,
    A8_UNORM = 65,
    R1_UNORM = 66,
    R9G9B9E5_SHAREDEXP = 67,
    R8G8_B8G8_UNORM = 68,
    G8R8_G8B8_UNORM = 69,
    BC1_TYPELESS = 70,
    BC1_UNORM = 71,
    BC1_UNORM_SRGB = 72,
    BC2_TYPELESS = 73,
    BC2_UNORM = 74,
    BC2_UNORM_SRGB = 75,
    BC3_TYPELESS = 76,
    BC3_UNORM = 77,
    BC3_UNORM_SRGB = 78,
    BC4_TYPELESS = 79,
    BC4_UNORM = 80,
    BC4_SNORM = 81,
    BC5_TYPELESS = 82,
    BC5_UNORM = 83,
    BC5_SNORM = 84,
    B5G6R5_UNORM = 85,
    B5G5R5A1_UNORM = 86,
    B8G8R8A8_UNORM = 87,
    B8G8R8X8_UNORM = 88,
    R10G10B10_XR_BIAS_A2_UNORM = 89,
    B8G8R8A8_TYPELESS = 90,
    B8G8R8A8_UNORM_SRGB = 91,
    B8G8R8X8_TYPELESS = 92,
    B8G8R8X8_UNORM_SRGB = 93,
    BC6H_TYPELESS = 94,
    BC6H_UF16 = 95,
    BC6H_SF16 = 96,
    BC7_TYPELESS = 97,
    BC7_UNORM = 98,
    BC7_UNORM_SRGB = 99,
    AYUV = 100,
    Y410 = 101,
    Y416 = 102,
    NV12 = 103,
    P010 = 104,
    P016 = 105,
    YUV_420_OPAQUE = 106,
    YUY2 = 107,
    Y210 = 108,
    Y216 = 109,
    NV11 = 110,
    AI44 = 111,
    IA44 = 112,
    P8 = 113,
    A8P8 = 114,
    B4G4R4A4_UNORM = 115,
    P208 = 130,
    V208 = 131,
    V408 = 132,
    SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
    SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,

    // Custom formats
    FIRST_CUSTOM_FORMAT = 65536,
    R8G8B8_UNORM = FIRST_CUSTOM_FORMAT,
    R8G8B8X8_UNORM,
};

enum struct AlphaType : std::uint8_t
{
    NO_ALPHA,
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
    TextureFormat format{};
    TextureType type{};
    AlphaType alpha_type{};
};

struct TextureData
{
    std::unique_ptr<std::byte[]> bytes;
    std::uint32_t row_pitch{};
    std::uint32_t row_count{};
    TextureDesc desc;
};

std::size_t get_texture_base_image_size(const TextureData &texture_data);

bool is_compressed_texture_format(TextureFormat format);

struct MipmapLevel
{
    std::span<std::byte> bytes;
    std::uint32_t row_pitch{};
    std::uint32_t row_count{};
    std::uint32_t width{};
    std::uint32_t height{};
    std::uint32_t depth{};
    std::uint32_t level{};
};

struct MipmapLevelIterator
{
    const TextureData &texture;
    std::uint32_t offset{};
    std::uint32_t level{};
};

bool operator==(const MipmapLevelIterator &it1, const MipmapLevelIterator &it2);
bool operator!=(const MipmapLevelIterator &it1, const MipmapLevelIterator &it2);
MipmapLevelIterator &operator++(MipmapLevelIterator &it);
MipmapLevel operator*(const MipmapLevelIterator &it);

MipmapLevelIterator begin(const TextureData &texture);
MipmapLevelIterator end(const TextureData &texture);

using TextureUnitId = std::uint16_t;
using TextureId = std::uint32_t;
using SamplerId = std::uint16_t;

inline constexpr TextureUnitId NULL_TEXTURE_UNIT_ID{std::numeric_limits<TextureUnitId>::max()};
inline constexpr TextureId NULL_TEXTURE_ID{std::numeric_limits<TextureId>::max()};
inline constexpr SamplerId NULL_SAMPLER_ID{std::numeric_limits<SamplerId>::max()};

struct TextureUnit
{
    TextureId bound_texture_id{NULL_TEXTURE_ID};
    std::uint32_t sampler_use_count{};
    SamplerId bound_sampler_id{NULL_SAMPLER_ID};
};

struct Texture
{
    GLuint texture{};
    TextureUnitId bound_texture_unit_id{NULL_TEXTURE_UNIT_ID};
    TextureType type{};
};

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
    SamplerFlags flags{};
};

struct Sampler
{
    GLuint sampler{};
    TextureUnitId bound_texture_unit_id{NULL_TEXTURE_UNIT_ID};
};

struct TextureLoadOptions
{
    bool force_srgb_internal_format : 1 {};
};

void init_texture_system();

void load_texture(TextureId texture_id, const TextureData &texture_data, TextureLoadOptions load_options = {});

void create_sampler(SamplerId sampler_id, const SamplerParams &sampler_params);
const SamplerParams &get_sampler_params(SamplerId sampler_id);
void set_sampler_params(SamplerId sampler_id, const SamplerParams &sampler_params);

// The shader program corresponding to the uniform needs to be in use before the function call.
void bind_texture_and_sampler(SamplerUniform &sampler_uniform, TextureId texture_id, SamplerId sampler_id);
} // namespace Age::Gfx
