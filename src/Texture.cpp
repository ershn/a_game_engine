#include <algorithm>
#include <vector>

#include "ErrorHandling.hpp"
#include "Texture.hpp"
#include "Utils.hpp"

namespace Age::Gfx
{
float get_texture_filtering_max_max_anisotropy()
{
    GLfloat max_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);
    return static_cast<float>(max_anisotropy);
}

namespace
{
static constexpr GLenum s_texture_type_to_gl_enum[] = {GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D};

constexpr GLenum to_gl_enum(TextureType texture_type)
{
    return s_texture_type_to_gl_enum[static_cast<std::size_t>(texture_type)];
}

static constexpr GLenum s_texture_wrap_mode_to_gl_enum[] = {
    GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
};

constexpr GLenum to_gl_enum(TextureWrapMode texture_wrap_mode)
{
    return s_texture_wrap_mode_to_gl_enum[static_cast<std::size_t>(texture_wrap_mode)];
}

static constexpr GLenum s_texture_min_filter_to_gl_enum[] = {
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_LINEAR
};

constexpr GLenum to_gl_enum(TextureMinFilter texture_min_filter)
{
    return s_texture_min_filter_to_gl_enum[static_cast<std::size_t>(texture_min_filter)];
}

static constexpr GLenum s_texture_mag_filter_to_gl_enum[] = {GL_NEAREST, GL_LINEAR};

constexpr GLenum to_gl_enum(TextureMagFilter texture_mag_filter)
{
    return s_texture_mag_filter_to_gl_enum[static_cast<std::size_t>(texture_mag_filter)];
}

struct PixelDataFormat
{
    GLenum format{};
    GLenum type{};
};

GLint s_texture_internal_formats_dxgi[] = {
    0,                                      // UNKNOWN
    0,                                      // R32G32B32A32_TYPELESS
    0,                                      // R32G32B32A32_FLOAT
    0,                                      // R32G32B32A32_UINT
    0,                                      // R32G32B32A32_SINT
    0,                                      // R32G32B32_TYPELESS
    0,                                      // R32G32B32_FLOAT
    0,                                      // R32G32B32_UINT
    0,                                      // R32G32B32_SINT
    0,                                      // R16G16B16A16_TYPELESS
    0,                                      // R16G16B16A16_FLOAT
    0,                                      // R16G16B16A16_UNORM
    0,                                      // R16G16B16A16_UINT
    0,                                      // R16G16B16A16_SNORM
    0,                                      // R16G16B16A16_SINT
    0,                                      // R32G32_TYPELESS
    0,                                      // R32G32_FLOAT
    0,                                      // R32G32_UINT
    0,                                      // R32G32_SINT
    0,                                      // R32G8X24_TYPELESS
    0,                                      // D32_FLOAT_S8X24_UINT
    0,                                      // R32_FLOAT_X8X24_TYPELESS
    0,                                      // X32_TYPELESS_G8X24_UINT
    0,                                      // R10G10B10A2_TYPELESS
    0,                                      // R10G10B10A2_UNORM
    0,                                      // R10G10B10A2_UINT
    0,                                      // R11G11B10_FLOAT
    0,                                      // R8G8B8A8_TYPELESS
    GL_RGBA8,                               // R8G8B8A8_UNORM
    GL_SRGB8_ALPHA8,                        // R8G8B8A8_UNORM_SRGB
    0,                                      // R8G8B8A8_UINT
    0,                                      // R8G8B8A8_SNORM
    0,                                      // R8G8B8A8_SINT
    0,                                      // R16G16_TYPELESS
    0,                                      // R16G16_FLOAT
    0,                                      // R16G16_UNORM
    0,                                      // R16G16_UINT
    0,                                      // R16G16_SNORM
    0,                                      // R16G16_SINT
    0,                                      // R32_TYPELESS
    0,                                      // D32_FLOAT
    0,                                      // R32_FLOAT
    0,                                      // R32_UINT
    0,                                      // R32_SINT
    0,                                      // R24G8_TYPELESS
    0,                                      // D24_UNORM_S8_UINT
    0,                                      // R24_UNORM_X8_TYPELESS
    0,                                      // X24_TYPELESS_G8_UINT
    0,                                      // R8G8_TYPELESS
    0,                                      // R8G8_UNORM
    0,                                      // R8G8_UINT
    0,                                      // R8G8_SNORM
    0,                                      // R8G8_SINT
    0,                                      // R16_TYPELESS
    0,                                      // R16_FLOAT
    0,                                      // D16_UNORM
    0,                                      // R16_UNORM
    0,                                      // R16_UINT
    0,                                      // R16_SNORM
    0,                                      // R16_SINT
    0,                                      // R8_TYPELESS
    GL_R8,                                  // R8_UNORM
    0,                                      // R8_UINT
    0,                                      // R8_SNORM
    0,                                      // R8_SINT
    0,                                      // A8_UNORM
    0,                                      // R1_UNORM
    0,                                      // R9G9B9E5_SHAREDEXP
    0,                                      // R8G8_B8G8_UNORM
    0,                                      // G8R8_G8B8_UNORM
    0,                                      // BC1_TYPELESS
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,        // BC1_UNORM
    GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,       // BC1_UNORM_SRGB
    0,                                      // BC2_TYPELESS
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,       // BC2_UNORM
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, // BC2_UNORM_SRGB
    0,                                      // BC3_TYPELESS
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,       // BC3_UNORM
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, // BC3_UNORM_SRGB
    0,                                      // BC4_TYPELESS
    0,                                      // BC4_UNORM
    0,                                      // BC4_SNORM
    0,                                      // BC5_TYPELESS
    0,                                      // BC5_UNORM
    0,                                      // BC5_SNORM
    0,                                      // B5G6R5_UNORM
    0,                                      // B5G5R5A1_UNORM
    GL_RGBA8,                               // B8G8R8A8_UNORM
    GL_RGB8,                                // B8G8R8X8_UNORM
    0,                                      // R10G10B10_XR_BIAS_A2_UNORM
    0,                                      // B8G8R8A8_TYPELESS
    GL_SRGB8_ALPHA8,                        // B8G8R8A8_UNORM_SRGB
    0,                                      // B8G8R8X8_TYPELESS
    GL_SRGB8,                               // B8G8R8X8_UNORM_SRGB
    0,                                      // BC6H_TYPELESS
    0,                                      // BC6H_UF16
    0,                                      // BC6H_SF16
    0,                                      // BC7_TYPELESS
    0,                                      // BC7_UNORM
    0,                                      // BC7_UNORM_SRGB
    0,                                      // AYUV
    0,                                      // Y410
    0,                                      // Y416
    0,                                      // NV12
    0,                                      // P010
    0,                                      // P016
    0,                                      // YUV_420_OPAQUE
    0,                                      // YUY2
    0,                                      // Y210
    0,                                      // Y216
    0,                                      // NV11
    0,                                      // AI44
    0,                                      // IA44
    0,                                      // P8
    0,                                      // A8P8
    0,                                      // B4G4R4A4_UNORM
};

GLint s_texture_internal_formats_custom[] = {
    GL_RGB8, // R8G8B8_UNORM
    GL_RGB8, // R8G8B8X8_UNORM
};

GLint s_texture_srgb_internal_formats_dxgi[] = {
    0,                                      // UNKNOWN
    0,                                      // R32G32B32A32_TYPELESS
    0,                                      // R32G32B32A32_FLOAT
    0,                                      // R32G32B32A32_UINT
    0,                                      // R32G32B32A32_SINT
    0,                                      // R32G32B32_TYPELESS
    0,                                      // R32G32B32_FLOAT
    0,                                      // R32G32B32_UINT
    0,                                      // R32G32B32_SINT
    0,                                      // R16G16B16A16_TYPELESS
    0,                                      // R16G16B16A16_FLOAT
    0,                                      // R16G16B16A16_UNORM
    0,                                      // R16G16B16A16_UINT
    0,                                      // R16G16B16A16_SNORM
    0,                                      // R16G16B16A16_SINT
    0,                                      // R32G32_TYPELESS
    0,                                      // R32G32_FLOAT
    0,                                      // R32G32_UINT
    0,                                      // R32G32_SINT
    0,                                      // R32G8X24_TYPELESS
    0,                                      // D32_FLOAT_S8X24_UINT
    0,                                      // R32_FLOAT_X8X24_TYPELESS
    0,                                      // X32_TYPELESS_G8X24_UINT
    0,                                      // R10G10B10A2_TYPELESS
    0,                                      // R10G10B10A2_UNORM
    0,                                      // R10G10B10A2_UINT
    0,                                      // R11G11B10_FLOAT
    0,                                      // R8G8B8A8_TYPELESS
    GL_SRGB8_ALPHA8,                        // R8G8B8A8_UNORM
    GL_SRGB8_ALPHA8,                        // R8G8B8A8_UNORM_SRGB
    0,                                      // R8G8B8A8_UINT
    0,                                      // R8G8B8A8_SNORM
    0,                                      // R8G8B8A8_SINT
    0,                                      // R16G16_TYPELESS
    0,                                      // R16G16_FLOAT
    0,                                      // R16G16_UNORM
    0,                                      // R16G16_UINT
    0,                                      // R16G16_SNORM
    0,                                      // R16G16_SINT
    0,                                      // R32_TYPELESS
    0,                                      // D32_FLOAT
    0,                                      // R32_FLOAT
    0,                                      // R32_UINT
    0,                                      // R32_SINT
    0,                                      // R24G8_TYPELESS
    0,                                      // D24_UNORM_S8_UINT
    0,                                      // R24_UNORM_X8_TYPELESS
    0,                                      // X24_TYPELESS_G8_UINT
    0,                                      // R8G8_TYPELESS
    0,                                      // R8G8_UNORM
    0,                                      // R8G8_UINT
    0,                                      // R8G8_SNORM
    0,                                      // R8G8_SINT
    0,                                      // R16_TYPELESS
    0,                                      // R16_FLOAT
    0,                                      // D16_UNORM
    0,                                      // R16_UNORM
    0,                                      // R16_UINT
    0,                                      // R16_SNORM
    0,                                      // R16_SINT
    0,                                      // R8_TYPELESS
    0,                                      // R8_UNORM
    0,                                      // R8_UINT
    0,                                      // R8_SNORM
    0,                                      // R8_SINT
    0,                                      // A8_UNORM
    0,                                      // R1_UNORM
    0,                                      // R9G9B9E5_SHAREDEXP
    0,                                      // R8G8_B8G8_UNORM
    0,                                      // G8R8_G8B8_UNORM
    0,                                      // BC1_TYPELESS
    GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,       // BC1_UNORM
    GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,       // BC1_UNORM_SRGB
    0,                                      // BC2_TYPELESS
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, // BC2_UNORM
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, // BC2_UNORM_SRGB
    0,                                      // BC3_TYPELESS
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, // BC3_UNORM
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, // BC3_UNORM_SRGB
    0,                                      // BC4_TYPELESS
    0,                                      // BC4_UNORM
    0,                                      // BC4_SNORM
    0,                                      // BC5_TYPELESS
    0,                                      // BC5_UNORM
    0,                                      // BC5_SNORM
    0,                                      // B5G6R5_UNORM
    0,                                      // B5G5R5A1_UNORM
    GL_SRGB8_ALPHA8,                        // B8G8R8A8_UNORM
    GL_SRGB8,                               // B8G8R8X8_UNORM
    0,                                      // R10G10B10_XR_BIAS_A2_UNORM
    0,                                      // B8G8R8A8_TYPELESS
    GL_SRGB8_ALPHA8,                        // B8G8R8A8_UNORM_SRGB
    0,                                      // B8G8R8X8_TYPELESS
    GL_SRGB8,                               // B8G8R8X8_UNORM_SRGB
    0,                                      // BC6H_TYPELESS
    0,                                      // BC6H_UF16
    0,                                      // BC6H_SF16
    0,                                      // BC7_TYPELESS
    0,                                      // BC7_UNORM
    0,                                      // BC7_UNORM_SRGB
    0,                                      // AYUV
    0,                                      // Y410
    0,                                      // Y416
    0,                                      // NV12
    0,                                      // P010
    0,                                      // P016
    0,                                      // YUV_420_OPAQUE
    0,                                      // YUY2
    0,                                      // Y210
    0,                                      // Y216
    0,                                      // NV11
    0,                                      // AI44
    0,                                      // IA44
    0,                                      // P8
    0,                                      // A8P8
    0,                                      // B4G4R4A4_UNORM
};

GLint s_texture_srgb_internal_formats_custom[] = {
    GL_SRGB8, // R8G8B8_UNORM
    GL_SRGB8, // R8G8B8X8_UNORM
};

PixelDataFormat s_texture_pixel_data_formats_dxgi[] = {
    {},                                                     // UNKNOWN
    {.format{}, .type{}},                                   // R32G32B32A32_TYPELESS
    {.format{}, .type{}},                                   // R32G32B32A32_FLOAT
    {.format{}, .type{}},                                   // R32G32B32A32_UINT
    {.format{}, .type{}},                                   // R32G32B32A32_SINT
    {.format{}, .type{}},                                   // R32G32B32_TYPELESS
    {.format{}, .type{}},                                   // R32G32B32_FLOAT
    {.format{}, .type{}},                                   // R32G32B32_UINT
    {.format{}, .type{}},                                   // R32G32B32_SINT
    {.format{}, .type{}},                                   // R16G16B16A16_TYPELESS
    {.format{}, .type{}},                                   // R16G16B16A16_FLOAT
    {.format{}, .type{}},                                   // R16G16B16A16_UNORM
    {.format{}, .type{}},                                   // R16G16B16A16_UINT
    {.format{}, .type{}},                                   // R16G16B16A16_SNORM
    {.format{}, .type{}},                                   // R16G16B16A16_SINT
    {.format{}, .type{}},                                   // R32G32_TYPELESS
    {.format{}, .type{}},                                   // R32G32_FLOAT
    {.format{}, .type{}},                                   // R32G32_UINT
    {.format{}, .type{}},                                   // R32G32_SINT
    {.format{}, .type{}},                                   // R32G8X24_TYPELESS
    {.format{}, .type{}},                                   // D32_FLOAT_S8X24_UINT
    {.format{}, .type{}},                                   // R32_FLOAT_X8X24_TYPELESS
    {.format{}, .type{}},                                   // X32_TYPELESS_G8X24_UINT
    {.format{}, .type{}},                                   // R10G10B10A2_TYPELESS
    {.format{}, .type{}},                                   // R10G10B10A2_UNORM
    {.format{}, .type{}},                                   // R10G10B10A2_UINT
    {.format{}, .type{}},                                   // R11G11B10_FLOAT
    {.format{}, .type{}},                                   // R8G8B8A8_TYPELESS
    {.format{GL_RGBA}, .type{GL_UNSIGNED_INT_8_8_8_8_REV}}, // R8G8B8A8_UNORM
    {.format{GL_RGBA}, .type{GL_UNSIGNED_INT_8_8_8_8_REV}}, // R8G8B8A8_UNORM_SRGB
    {.format{}, .type{}},                                   // R8G8B8A8_UINT
    {.format{}, .type{}},                                   // R8G8B8A8_SNORM
    {.format{}, .type{}},                                   // R8G8B8A8_SINT
    {.format{}, .type{}},                                   // R16G16_TYPELESS
    {.format{}, .type{}},                                   // R16G16_FLOAT
    {.format{}, .type{}},                                   // R16G16_UNORM
    {.format{}, .type{}},                                   // R16G16_UINT
    {.format{}, .type{}},                                   // R16G16_SNORM
    {.format{}, .type{}},                                   // R16G16_SINT
    {.format{}, .type{}},                                   // R32_TYPELESS
    {.format{}, .type{}},                                   // D32_FLOAT
    {.format{}, .type{}},                                   // R32_FLOAT
    {.format{}, .type{}},                                   // R32_UINT
    {.format{}, .type{}},                                   // R32_SINT
    {.format{}, .type{}},                                   // R24G8_TYPELESS
    {.format{}, .type{}},                                   // D24_UNORM_S8_UINT
    {.format{}, .type{}},                                   // R24_UNORM_X8_TYPELESS
    {.format{}, .type{}},                                   // X24_TYPELESS_G8_UINT
    {.format{}, .type{}},                                   // R8G8_TYPELESS
    {.format{}, .type{}},                                   // R8G8_UNORM
    {.format{}, .type{}},                                   // R8G8_UINT
    {.format{}, .type{}},                                   // R8G8_SNORM
    {.format{}, .type{}},                                   // R8G8_SINT
    {.format{}, .type{}},                                   // R16_TYPELESS
    {.format{}, .type{}},                                   // R16_FLOAT
    {.format{}, .type{}},                                   // D16_UNORM
    {.format{}, .type{}},                                   // R16_UNORM
    {.format{}, .type{}},                                   // R16_UINT
    {.format{}, .type{}},                                   // R16_SNORM
    {.format{}, .type{}},                                   // R16_SINT
    {.format{}, .type{}},                                   // R8_TYPELESS
    {.format{GL_RED}, .type{GL_UNSIGNED_BYTE}},             // R8_UNORM
    {.format{}, .type{}},                                   // R8_UINT
    {.format{}, .type{}},                                   // R8_SNORM
    {.format{}, .type{}},                                   // R8_SINT
    {.format{}, .type{}},                                   // A8_UNORM
    {.format{}, .type{}},                                   // R1_UNORM
    {.format{}, .type{}},                                   // R9G9B9E5_SHAREDEXP
    {.format{}, .type{}},                                   // R8G8_B8G8_UNORM
    {.format{}, .type{}},                                   // G8R8_G8B8_UNORM
    {},                                                     // BC1_TYPELESS
    {},                                                     // BC1_UNORM
    {},                                                     // BC1_UNORM_SRGB
    {},                                                     // BC2_TYPELESS
    {},                                                     // BC2_UNORM
    {},                                                     // BC2_UNORM_SRGB
    {},                                                     // BC3_TYPELESS
    {},                                                     // BC3_UNORM
    {},                                                     // BC3_UNORM_SRGB
    {},                                                     // BC4_TYPELESS
    {},                                                     // BC4_UNORM
    {},                                                     // BC4_SNORM
    {},                                                     // BC5_TYPELESS
    {},                                                     // BC5_UNORM
    {},                                                     // BC5_SNORM
    {.format{}, .type{}},                                   // B5G6R5_UNORM
    {.format{}, .type{}},                                   // B5G5R5A1_UNORM
    {.format{GL_BGRA}, .type{GL_UNSIGNED_INT_8_8_8_8_REV}}, // B8G8R8A8_UNORM
    {.format{GL_BGRA}, .type{GL_UNSIGNED_INT_8_8_8_8_REV}}, // B8G8R8X8_UNORM
    {.format{}, .type{}},                                   // R10G10B10_XR_BIAS_A2_UNORM
    {.format{}, .type{}},                                   // B8G8R8A8_TYPELESS
    {.format{GL_BGRA}, .type{GL_UNSIGNED_INT_8_8_8_8_REV}}, // B8G8R8A8_UNORM_SRGB
    {.format{}, .type{}},                                   // B8G8R8X8_TYPELESS
    {.format{GL_BGRA}, .type{GL_UNSIGNED_INT_8_8_8_8_REV}}, // B8G8R8X8_UNORM_SRGB
    {},                                                     // BC6H_TYPELESS
    {},                                                     // BC6H_UF16
    {},                                                     // BC6H_SF16
    {},                                                     // BC7_TYPELESS
    {},                                                     // BC7_UNORM
    {},                                                     // BC7_UNORM_SRGB
    {.format{}, .type{}},                                   // AYUV
    {.format{}, .type{}},                                   // Y410
    {.format{}, .type{}},                                   // Y416
    {.format{}, .type{}},                                   // NV12
    {.format{}, .type{}},                                   // P010
    {.format{}, .type{}},                                   // P016
    {.format{}, .type{}},                                   // YUV_420_OPAQUE
    {.format{}, .type{}},                                   // YUY2
    {.format{}, .type{}},                                   // Y210
    {.format{}, .type{}},                                   // Y216
    {.format{}, .type{}},                                   // NV11
    {.format{}, .type{}},                                   // AI44
    {.format{}, .type{}},                                   // IA44
    {.format{}, .type{}},                                   // P8
    {.format{}, .type{}},                                   // A8P8
    {.format{}, .type{}},                                   // B4G4R4A4_UNORM
};

PixelDataFormat s_texture_pixel_data_formats_custom[] = {
    {.format{GL_RGB}, .type{GL_UNSIGNED_BYTE}},             // R8G8B8_UNORM
    {.format{GL_RGBA}, .type{GL_UNSIGNED_INT_8_8_8_8_REV}}, // R8G8B8X8_UNORM
};

GLint get_texture_internal_format(TextureFormat format, bool force_srgb_internal_format)
{
    GLint internal_format{};

    auto format_index = Util::to_underlying(format);
    if (format_index < Util::to_underlying(TextureFormat::FIRST_CUSTOM_FORMAT))
    {
        if (format_index < std::size(s_texture_internal_formats_dxgi))
        {
            if (force_srgb_internal_format)
                internal_format = s_texture_srgb_internal_formats_dxgi[format_index];
            else
                internal_format = s_texture_internal_formats_dxgi[format_index];
        }
    }
    else
    {
        auto offset = format_index - Util::to_underlying(TextureFormat::FIRST_CUSTOM_FORMAT);
        if (offset < std::size(s_texture_internal_formats_custom))
        {
            if (force_srgb_internal_format)
                internal_format = s_texture_srgb_internal_formats_custom[offset];
            else
                internal_format = s_texture_internal_formats_custom[offset];
        }
    }

    LOG_ERROR_IF(
        internal_format == 0,
        "no internal format found for texture format: {} (force_srgb_internal_format = {})",
        format_index,
        force_srgb_internal_format
    );
    return internal_format;
}

PixelDataFormat get_pixel_data_format(TextureFormat format)
{
    PixelDataFormat pixel_data_format{};

    auto format_index = Util::to_underlying(format);
    if (format_index < Util::to_underlying(TextureFormat::FIRST_CUSTOM_FORMAT))
    {
        if (format_index < std::size(s_texture_pixel_data_formats_dxgi))
            pixel_data_format = s_texture_pixel_data_formats_dxgi[format_index];
    }
    else
    {
        auto offset = format_index - Util::to_underlying(TextureFormat::FIRST_CUSTOM_FORMAT);
        if (offset < std::size(s_texture_pixel_data_formats_custom))
            pixel_data_format = s_texture_pixel_data_formats_custom[offset];
    }

    LOG_ERROR_IF(pixel_data_format.format == 0, "no pixel data format found for texture format: {}", format_index);
    return pixel_data_format;
}

unsigned int get_row_alignment_from_pitch(std::uint32_t pitch)
{
    if ((pitch & 0b1111) == 0b1000)
        return 8;
    else if ((pitch & 0b111) == 0b100)
        return 4;
    else if ((pitch & 0b11) == 0b10)
        return 2;
    else
        return 1;
}
} // namespace

std::size_t get_texture_base_image_size(const TextureData &texture_data)
{
    return texture_data.row_pitch * texture_data.row_count * texture_data.desc.depth;
}

bool is_compressed_texture_format(TextureFormat format)
{
    auto format_value = Util::to_underlying(format);
    return Util::to_underlying(TextureFormat::BC1_TYPELESS) <= format_value &&
               format_value <= Util::to_underlying(TextureFormat::BC5_SNORM) ||
           Util::to_underlying(TextureFormat::BC6H_TYPELESS) <= format_value &&
               format_value <= Util::to_underlying(TextureFormat::BC7_UNORM_SRGB);
}

bool operator==(const MipmapLevelIterator &it1, const MipmapLevelIterator &it2)
{
    return &it1.texture == &it2.texture && it1.level == it2.level;
}

bool operator!=(const MipmapLevelIterator &it1, const MipmapLevelIterator &it2)
{
    return !(it1 == it2);
}

MipmapLevelIterator &operator++(MipmapLevelIterator &it)
{
    auto row_pitch = std::max(1U, it.texture.row_pitch >> it.level);
    auto row_count = std::max(1U, it.texture.row_count >> it.level);
    auto depth = std::max(1U, it.texture.desc.depth >> it.level);

    it.offset += row_pitch * row_count * depth;
    ++it.level;
    return it;
}

MipmapLevel operator*(const MipmapLevelIterator &it)
{
    auto row_pitch = std::max(1U, it.texture.row_pitch >> it.level);
    auto row_count = std::max(1U, it.texture.row_count >> it.level);
    auto width = std::max(1U, it.texture.desc.width >> it.level);
    auto height = std::max(1U, it.texture.desc.height >> it.level);
    auto depth = std::max(1U, it.texture.desc.depth >> it.level);

    return MipmapLevel{
        .bytes{&it.texture.bytes[it.offset], row_pitch * row_count * depth},
        .row_pitch{row_pitch},
        .row_count{row_count},
        .width{width},
        .height{height},
        .depth{depth},
        .level{it.level}
    };
}

MipmapLevelIterator begin(const TextureData &texture)
{
    return MipmapLevelIterator{.texture{texture}, .offset{0}, .level{0}};
}

MipmapLevelIterator end(const TextureData &texture)
{
    return MipmapLevelIterator{.texture{texture}, .offset{}, .level{texture.desc.mipmap_level_count}};
}

namespace
{
std::vector<TextureUnit> s_texture_units;
std::vector<Texture> s_textures;
std::vector<TextureDesc> s_texture_descs;
std::vector<Sampler> s_samplers;
std::vector<SamplerParams> s_sampler_params;

unsigned int get_texture_unit_count()
{
    return static_cast<unsigned int>(OGL::get_integer(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS));
}

std::pair<TextureUnitId, TextureUnit &> get_least_used_texture_unit()
{
    TextureUnitId least_used_tex_unit_id{0};
    TextureUnit *least_used_tex_unit{&s_texture_units[0]};

    for (TextureUnitId tex_unit_id{1}; tex_unit_id < s_texture_units.size(); ++tex_unit_id)
    {
        auto &tex_unit = s_texture_units[tex_unit_id];

        if (least_used_tex_unit->bound_texture_id == NULL_TEXTURE_ID &&
                (tex_unit.bound_texture_id == NULL_TEXTURE_ID &&
                 tex_unit.sampler_use_count < least_used_tex_unit->sampler_use_count) ||
            least_used_tex_unit->bound_texture_id != NULL_TEXTURE_ID &&
                (tex_unit.bound_texture_id == NULL_TEXTURE_ID ||
                 tex_unit.sampler_use_count < least_used_tex_unit->sampler_use_count))
        {
            least_used_tex_unit_id = tex_unit_id;
            least_used_tex_unit = &tex_unit;
        }
    }

    return {least_used_tex_unit_id, s_texture_units[least_used_tex_unit_id]};
}

GLint get_pixel_data_unpack_alignment()
{
    return OGL::get_integer(GL_UNPACK_ALIGNMENT);
}

void update_pixel_data_unpack_alignment(std::uint32_t pitch, GLint &unpack_alignment)
{
    GLint row_alignment{static_cast<GLint>(get_row_alignment_from_pitch(pitch))};
    if (row_alignment != unpack_alignment)
    {
        unpack_alignment = row_alignment;
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);
    }
}

GLuint load_texture_1d(const TextureData &texture_data, TextureUnitId texture_unit_id, TextureLoadOptions load_options)
{
    GLuint texture;

    auto internal_format =
        get_texture_internal_format(texture_data.desc.format, load_options.force_srgb_internal_format);
    auto data_format = get_pixel_data_format(texture_data.desc.format);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
    glBindTexture(GL_TEXTURE_1D, texture);

    GLint unpack_alignment{get_pixel_data_unpack_alignment()};

    if (texture_data.desc.mipmap_level_count > 1)
    {
        for (const auto &mipmap_level : texture_data)
        {
            update_pixel_data_unpack_alignment(mipmap_level.row_pitch, unpack_alignment);

            glTexImage1D(
                GL_TEXTURE_1D,
                mipmap_level.level,
                internal_format,
                mipmap_level.width,
                0,
                data_format.format,
                data_format.type,
                mipmap_level.bytes.data()
            );
        }
    }
    else
    {
        update_pixel_data_unpack_alignment(texture_data.row_pitch, unpack_alignment);

        glTexImage1D(
            GL_TEXTURE_1D,
            0,
            internal_format,
            texture_data.desc.width,
            0,
            data_format.format,
            data_format.type,
            texture_data.bytes.get()
        );
    }

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, texture_data.desc.mipmap_level_count - 1);

    glBindTexture(GL_TEXTURE_1D, 0);

    return texture;
}

GLuint load_texture_2d(const TextureData &texture_data, TextureUnitId texture_unit_id, TextureLoadOptions load_options)
{
    GLuint texture;

    auto internal_format =
        get_texture_internal_format(texture_data.desc.format, load_options.force_srgb_internal_format);
    auto data_format = get_pixel_data_format(texture_data.desc.format);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLint unpack_alignment{get_pixel_data_unpack_alignment()};

    if (texture_data.desc.mipmap_level_count > 1)
    {
        for (const auto &mipmap_level : texture_data)
        {
            update_pixel_data_unpack_alignment(mipmap_level.row_pitch, unpack_alignment);

            glTexImage2D(
                GL_TEXTURE_2D,
                mipmap_level.level,
                internal_format,
                mipmap_level.width,
                mipmap_level.height,
                0,
                data_format.format,
                data_format.type,
                mipmap_level.bytes.data()
            );
        }
    }
    else
    {
        update_pixel_data_unpack_alignment(texture_data.row_pitch, unpack_alignment);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internal_format,
            texture_data.desc.width,
            texture_data.desc.height,
            0,
            data_format.format,
            data_format.type,
            texture_data.bytes.get()
        );
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, texture_data.desc.mipmap_level_count - 1);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

GLuint load_texture_3d(const TextureData &texture_data, TextureUnitId texture_unit_id, TextureLoadOptions load_options)
{
    GLuint texture;

    auto internal_format =
        get_texture_internal_format(texture_data.desc.format, load_options.force_srgb_internal_format);
    auto data_format = get_pixel_data_format(texture_data.desc.format);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
    glBindTexture(GL_TEXTURE_3D, texture);

    GLint unpack_alignment{get_pixel_data_unpack_alignment()};

    if (texture_data.desc.mipmap_level_count > 1)
    {
        for (const auto &mipmap_level : texture_data)
        {
            update_pixel_data_unpack_alignment(mipmap_level.row_pitch, unpack_alignment);

            glTexImage3D(
                GL_TEXTURE_3D,
                mipmap_level.level,
                internal_format,
                mipmap_level.width,
                mipmap_level.height,
                mipmap_level.depth,
                0,
                data_format.format,
                data_format.type,
                mipmap_level.bytes.data()
            );
        }
    }
    else
    {
        update_pixel_data_unpack_alignment(texture_data.row_pitch, unpack_alignment);

        glTexImage3D(
            GL_TEXTURE_3D,
            0,
            internal_format,
            texture_data.desc.width,
            texture_data.desc.height,
            texture_data.desc.depth,
            0,
            data_format.format,
            data_format.type,
            texture_data.bytes.get()
        );
    }

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, texture_data.desc.mipmap_level_count - 1);

    glBindTexture(GL_TEXTURE_3D, 0);

    return texture;
}

GLuint load_compressed_texture_1d(
    const TextureData &texture_data, TextureUnitId texture_unit_id, TextureLoadOptions load_options
)
{
    GLuint texture;

    auto internal_format =
        get_texture_internal_format(texture_data.desc.format, load_options.force_srgb_internal_format);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
    glBindTexture(GL_TEXTURE_1D, texture);

    if (texture_data.desc.mipmap_level_count > 1)
    {
        for (const auto &mipmap_level : texture_data)
        {
            glCompressedTexImage1D(
                GL_TEXTURE_1D,
                static_cast<GLint>(mipmap_level.level),
                static_cast<GLenum>(internal_format),
                static_cast<GLsizei>(mipmap_level.width),
                0,
                static_cast<GLsizei>(mipmap_level.bytes.size()),
                mipmap_level.bytes.data()
            );
        }
    }
    else
    {
        glCompressedTexImage1D(
            GL_TEXTURE_1D,
            0,
            static_cast<GLenum>(internal_format),
            static_cast<GLsizei>(texture_data.desc.width),
            0,
            static_cast<GLsizei>(get_texture_base_image_size(texture_data)),
            texture_data.bytes.get()
        );
    }

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, texture_data.desc.mipmap_level_count - 1);

    glBindTexture(GL_TEXTURE_1D, 0);

    return texture;
}

GLuint load_compressed_texture_2d(
    const TextureData &texture_data, TextureUnitId texture_unit_id, TextureLoadOptions load_options
)
{
    GLuint texture;

    auto internal_format =
        get_texture_internal_format(texture_data.desc.format, load_options.force_srgb_internal_format);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (texture_data.desc.mipmap_level_count > 1)
    {
        for (const auto &mipmap_level : texture_data)
        {
            glCompressedTexImage2D(
                GL_TEXTURE_2D,
                static_cast<GLint>(mipmap_level.level),
                static_cast<GLenum>(internal_format),
                static_cast<GLsizei>(mipmap_level.width),
                static_cast<GLsizei>(mipmap_level.height),
                0,
                static_cast<GLsizei>(mipmap_level.bytes.size()),
                mipmap_level.bytes.data()
            );
        }
    }
    else
    {
        glCompressedTexImage2D(
            GL_TEXTURE_2D,
            0,
            static_cast<GLenum>(internal_format),
            static_cast<GLsizei>(texture_data.desc.width),
            static_cast<GLsizei>(texture_data.desc.height),
            0,
            static_cast<GLsizei>(get_texture_base_image_size(texture_data)),
            texture_data.bytes.get()
        );
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, texture_data.desc.mipmap_level_count - 1);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

GLuint load_compressed_texture_3d(
    const TextureData &texture_data, TextureUnitId texture_unit_id, TextureLoadOptions load_options
)
{
    GLuint texture;

    auto internal_format =
        get_texture_internal_format(texture_data.desc.format, load_options.force_srgb_internal_format);

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
    glBindTexture(GL_TEXTURE_3D, texture);

    if (texture_data.desc.mipmap_level_count > 1)
    {
        for (const auto &mipmap_level : texture_data)
        {
            glCompressedTexImage3D(
                GL_TEXTURE_3D,
                static_cast<GLint>(mipmap_level.level),
                static_cast<GLenum>(internal_format),
                static_cast<GLsizei>(mipmap_level.width),
                static_cast<GLsizei>(mipmap_level.height),
                static_cast<GLsizei>(mipmap_level.depth),
                0,
                static_cast<GLsizei>(mipmap_level.bytes.size()),
                mipmap_level.bytes.data()
            );
        }
    }
    else
    {
        glCompressedTexImage3D(
            GL_TEXTURE_3D,
            0,
            static_cast<GLenum>(internal_format),
            static_cast<GLsizei>(texture_data.desc.width),
            static_cast<GLsizei>(texture_data.desc.height),
            static_cast<GLsizei>(texture_data.desc.depth),
            0,
            static_cast<GLsizei>(get_texture_base_image_size(texture_data)),
            texture_data.bytes.get()
        );
    }

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, texture_data.desc.mipmap_level_count - 1);

    glBindTexture(GL_TEXTURE_3D, 0);

    return texture;
}

void bind_texture(TextureId texture_id, Texture &texture, TextureUnitId texture_unit_id, TextureUnit &texture_unit)
{
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
    glBindTexture(to_gl_enum(texture.type), texture.texture);

    texture.bound_texture_unit_id = texture_unit_id;
    texture_unit.bound_texture_id = texture_id;
}

void unbind_texture(TextureUnit &texture_unit)
{
    if (texture_unit.bound_texture_id == NULL_TEXTURE_ID)
        return;

    s_textures[texture_unit.bound_texture_id].bound_texture_unit_id = NULL_TEXTURE_UNIT_ID;
    texture_unit.bound_texture_id = NULL_TEXTURE_ID;
}

void bind_sampler_uniform(SamplerUniform &sampler_uniform, TextureUnitId texture_unit_id, TextureUnit &texture_unit)
{
    TextureUnitId bound_texture_unit_id{sampler_uniform.bound_texture_unit_id};

    if (bound_texture_unit_id == texture_unit_id)
        return;

    if (bound_texture_unit_id != NULL_TEXTURE_UNIT_ID)
        --s_texture_units[bound_texture_unit_id].sampler_use_count;

    OGL::set_uniform(sampler_uniform.uniform, texture_unit_id);

    sampler_uniform.bound_texture_unit_id = texture_unit_id;
    ++texture_unit.sampler_use_count;
}

// Default OpenGL sampler parameter values
constexpr SamplerParams DEFAULT_SAMPLER_PARAMS{
    .max_anisotropy{1.0f},
    .flags{
        .texture_wrap_s{TextureWrapMode::REPEAT},
        .texture_wrap_t{TextureWrapMode::REPEAT},
        .texture_wrap_r{TextureWrapMode::REPEAT},
        .texture_mag_filter{TextureMagFilter::LINEAR},
        .texture_min_filter{TextureMinFilter::NEAREST_MIPMAP_LINEAR}
    }
};

void update_sampler_params(GLuint sampler, const SamplerParams &params, const SamplerParams &prev_params)
{
    if (params.max_anisotropy != prev_params.max_anisotropy)
        glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, params.max_anisotropy);

    if (params.flags.texture_wrap_s != prev_params.flags.texture_wrap_s)
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, to_gl_enum(params.flags.texture_wrap_s));

    if (params.flags.texture_wrap_t != prev_params.flags.texture_wrap_t)
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, to_gl_enum(params.flags.texture_wrap_t));

    if (params.flags.texture_wrap_r != prev_params.flags.texture_wrap_r)
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, to_gl_enum(params.flags.texture_wrap_r));

    if (params.flags.texture_min_filter != prev_params.flags.texture_min_filter)
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, to_gl_enum(params.flags.texture_min_filter));

    if (params.flags.texture_mag_filter != prev_params.flags.texture_mag_filter)
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, to_gl_enum(params.flags.texture_mag_filter));
}

void bind_sampler(SamplerId sampler_id, Sampler &sampler, TextureUnitId texture_unit_id, TextureUnit &texture_unit)
{
    glBindSampler(static_cast<GLuint>(texture_unit_id), sampler.sampler);

    sampler.bound_texture_unit_id = texture_unit_id;
    texture_unit.bound_sampler_id = sampler_id;
}

void unbind_sampler(TextureUnit &texture_unit)
{
    if (texture_unit.bound_sampler_id == NULL_SAMPLER_ID)
        return;

    s_samplers[texture_unit.bound_sampler_id].bound_texture_unit_id = NULL_TEXTURE_UNIT_ID;
    texture_unit.bound_sampler_id = NULL_SAMPLER_ID;
}
} // namespace

void init_texture_system()
{
    auto texture_unit_count = get_texture_unit_count();

    s_texture_units.resize(texture_unit_count);
    s_textures.reserve(256);
    s_texture_descs.reserve(256);
    s_samplers.reserve(64);
    s_sampler_params.reserve(64);
}

void load_texture(TextureId texture_id, const TextureData &texture_data, TextureLoadOptions load_options)
{
    if (texture_id >= s_textures.size())
    {
        s_textures.resize(texture_id + 1);
        s_texture_descs.resize(texture_id + 1);
    }

    s_texture_descs[texture_id] = texture_data.desc;

    Texture &texture{s_textures[texture_id]};
    texture.type = texture_data.desc.type;

    auto [texture_unit_id, texture_unit] = get_least_used_texture_unit();
    unbind_texture(texture_unit);

    if (is_compressed_texture_format(texture_data.desc.format))
    {
        switch (texture_data.desc.type)
        {
        case TextureType::TEXTURE_1D:
            texture.texture = load_compressed_texture_1d(texture_data, texture_unit_id, load_options);
            break;
        case TextureType::TEXTURE_2D:
            texture.texture = load_compressed_texture_2d(texture_data, texture_unit_id, load_options);
            break;
        case TextureType::TEXTURE_3D:
            texture.texture = load_compressed_texture_3d(texture_data, texture_unit_id, load_options);
            break;
        }
    }
    else
    {
        switch (texture_data.desc.type)
        {
        case TextureType::TEXTURE_1D:
            texture.texture = load_texture_1d(texture_data, texture_unit_id, load_options);
            break;
        case TextureType::TEXTURE_2D:
            texture.texture = load_texture_2d(texture_data, texture_unit_id, load_options);
            break;
        case TextureType::TEXTURE_3D:
            texture.texture = load_texture_3d(texture_data, texture_unit_id, load_options);
            break;
        }
    }
}

void create_sampler(SamplerId sampler_id, const SamplerParams &sampler_params)
{
    if (sampler_id >= s_samplers.size())
    {
        s_samplers.resize(sampler_id + 1);
        s_sampler_params.resize(sampler_id + 1);
    }

    Sampler &sampler{s_samplers[sampler_id]};
    glGenSamplers(1, &sampler.sampler);
    update_sampler_params(sampler.sampler, sampler_params, DEFAULT_SAMPLER_PARAMS);
    s_sampler_params[sampler_id] = sampler_params;
}

const SamplerParams &get_sampler_params(SamplerId sampler_id)
{
    return s_sampler_params[sampler_id];
}

void set_sampler_params(SamplerId sampler_id, const SamplerParams &sampler_params)
{
    const Sampler &sampler{s_samplers[sampler_id]};
    SamplerParams &current_sampler_params{s_sampler_params[sampler_id]};
    update_sampler_params(sampler.sampler, sampler_params, current_sampler_params);
    current_sampler_params = sampler_params;
}

void bind_texture_and_sampler(SamplerUniform &sampler_uniform, TextureId texture_id, SamplerId sampler_id)
{
    Texture &texture{s_textures[texture_id]};
    TextureUnitId texture_unit_id{};
    TextureUnit *texture_unit{};

    if (texture.bound_texture_unit_id == NULL_TEXTURE_UNIT_ID)
    {
        auto [least_used_texture_unit_id, least_used_texture_unit] = get_least_used_texture_unit();
        unbind_texture(least_used_texture_unit);

        texture_unit_id = least_used_texture_unit_id;
        texture_unit = &least_used_texture_unit;
        bind_texture(texture_id, texture, texture_unit_id, *texture_unit);
        bind_sampler_uniform(sampler_uniform, texture_unit_id, *texture_unit);
    }
    else
    {
        texture_unit_id = texture.bound_texture_unit_id;
        texture_unit = &s_texture_units[texture_unit_id];
        bind_sampler_uniform(sampler_uniform, texture_unit_id, *texture_unit);
    }

    if (texture_unit->bound_sampler_id != sampler_id)
    {
        unbind_sampler(*texture_unit);

        Sampler &sampler{s_samplers[sampler_id]};
        bind_sampler(sampler_id, sampler, texture_unit_id, *texture_unit);
    }
}
} // namespace Age::Gfx
