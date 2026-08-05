#include "error_handling.hpp"
#include "utils.hpp"
#include "opengl/opengl_image_formats.hpp"

namespace Age::Gfx::OGL
{
namespace
{
GLint s_internal_formats_dxgi[] = {
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

GLint s_internal_formats_custom[] = {
    GL_RGB8, // R8G8B8_UNORM
    GL_RGB8, // R8G8B8X8_UNORM
};

GLint s_srgb_internal_formats_dxgi[] = {
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

GLint s_srgb_internal_formats_custom[] = {
    GL_SRGB8, // R8G8B8_UNORM
    GL_SRGB8, // R8G8B8X8_UNORM
};

PixelDataFormat s_pixel_data_formats_dxgi[] = {
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

PixelDataFormat s_pixel_data_formats_custom[] = {
    {.format{GL_RGB}, .type{GL_UNSIGNED_BYTE}},             // R8G8B8_UNORM
    {.format{GL_RGBA}, .type{GL_UNSIGNED_INT_8_8_8_8_REV}}, // R8G8B8X8_UNORM
};
} // namespace

GLint get_internal_format(ImageFormat format, bool force_srgb_internal_format)
{
    GLint internal_format{};

    auto format_index = Util::to_underlying(format);
    if (format_index < Util::to_underlying(ImageFormat::FIRST_CUSTOM_FORMAT))
    {
        if (format_index < std::size(s_internal_formats_dxgi))
        {
            if (force_srgb_internal_format)
                internal_format = s_srgb_internal_formats_dxgi[format_index];
            else
                internal_format = s_internal_formats_dxgi[format_index];
        }
    }
    else
    {
        auto offset = format_index - Util::to_underlying(ImageFormat::FIRST_CUSTOM_FORMAT);
        if (offset < std::size(s_internal_formats_custom))
        {
            if (force_srgb_internal_format)
                internal_format = s_srgb_internal_formats_custom[offset];
            else
                internal_format = s_internal_formats_custom[offset];
        }
    }

    LOG_ERROR_IF(
        internal_format == 0,
        "no internal format found for image format: {} (force_srgb_internal_format = {})",
        format_index,
        force_srgb_internal_format
    );
    return internal_format;
}

PixelDataFormat get_pixel_data_format(ImageFormat format)
{
    PixelDataFormat pixel_data_format{};

    auto format_index = Util::to_underlying(format);
    if (format_index < Util::to_underlying(ImageFormat::FIRST_CUSTOM_FORMAT))
    {
        if (format_index < std::size(s_pixel_data_formats_dxgi))
            pixel_data_format = s_pixel_data_formats_dxgi[format_index];
    }
    else
    {
        auto offset = format_index - Util::to_underlying(ImageFormat::FIRST_CUSTOM_FORMAT);
        if (offset < std::size(s_pixel_data_formats_custom))
            pixel_data_format = s_pixel_data_formats_custom[offset];
    }

    LOG_ERROR_IF(pixel_data_format.format == 0, "no pixel data format found for image format: {}", format_index);
    return pixel_data_format;
}
} // namespace Age::Gfx::OGL
