#include "image_formats.hpp"
#include "error_handling.hpp"
#include "utils.hpp"

namespace Age::Gfx
{
namespace
{
unsigned int s_pixel_data_bpp_dxgi[] = {
    0,  // UNKNOWN
    0,  // R32G32B32A32_TYPELESS
    0,  // R32G32B32A32_FLOAT
    0,  // R32G32B32A32_UINT
    0,  // R32G32B32A32_SINT
    0,  // R32G32B32_TYPELESS
    0,  // R32G32B32_FLOAT
    0,  // R32G32B32_UINT
    0,  // R32G32B32_SINT
    0,  // R16G16B16A16_TYPELESS
    0,  // R16G16B16A16_FLOAT
    0,  // R16G16B16A16_UNORM
    0,  // R16G16B16A16_UINT
    0,  // R16G16B16A16_SNORM
    0,  // R16G16B16A16_SINT
    0,  // R32G32_TYPELESS
    0,  // R32G32_FLOAT
    0,  // R32G32_UINT
    0,  // R32G32_SINT
    0,  // R32G8X24_TYPELESS
    0,  // D32_FLOAT_S8X24_UINT
    0,  // R32_FLOAT_X8X24_TYPELESS
    0,  // X32_TYPELESS_G8X24_UINT
    0,  // R10G10B10A2_TYPELESS
    0,  // R10G10B10A2_UNORM
    0,  // R10G10B10A2_UINT
    0,  // R11G11B10_FLOAT
    0,  // R8G8B8A8_TYPELESS
    32, // R8G8B8A8_UNORM
    32, // R8G8B8A8_UNORM_SRGB
    0,  // R8G8B8A8_UINT
    0,  // R8G8B8A8_SNORM
    0,  // R8G8B8A8_SINT
    0,  // R16G16_TYPELESS
    0,  // R16G16_FLOAT
    0,  // R16G16_UNORM
    0,  // R16G16_UINT
    0,  // R16G16_SNORM
    0,  // R16G16_SINT
    0,  // R32_TYPELESS
    0,  // D32_FLOAT
    0,  // R32_FLOAT
    0,  // R32_UINT
    0,  // R32_SINT
    0,  // R24G8_TYPELESS
    0,  // D24_UNORM_S8_UINT
    0,  // R24_UNORM_X8_TYPELESS
    0,  // X24_TYPELESS_G8_UINT
    0,  // R8G8_TYPELESS
    0,  // R8G8_UNORM
    0,  // R8G8_UINT
    0,  // R8G8_SNORM
    0,  // R8G8_SINT
    0,  // R16_TYPELESS
    0,  // R16_FLOAT
    16, // D16_UNORM
    0,  // R16_UNORM
    0,  // R16_UINT
    0,  // R16_SNORM
    0,  // R16_SINT
    0,  // R8_TYPELESS
    8,  // R8_UNORM
    0,  // R8_UINT
    0,  // R8_SNORM
    0,  // R8_SINT
    0,  // A8_UNORM
    0,  // R1_UNORM
    0,  // R9G9B9E5_SHAREDEXP
    0,  // R8G8_B8G8_UNORM
    0,  // G8R8_G8B8_UNORM
    0,  // BC1_TYPELESS
    0,  // BC1_UNORM
    0,  // BC1_UNORM_SRGB
    0,  // BC2_TYPELESS
    0,  // BC2_UNORM
    0,  // BC2_UNORM_SRGB
    0,  // BC3_TYPELESS
    0,  // BC3_UNORM
    0,  // BC3_UNORM_SRGB
    0,  // BC4_TYPELESS
    0,  // BC4_UNORM
    0,  // BC4_SNORM
    0,  // BC5_TYPELESS
    0,  // BC5_UNORM
    0,  // BC5_SNORM
    0,  // B5G6R5_UNORM
    0,  // B5G5R5A1_UNORM
    32, // B8G8R8A8_UNORM
    32, // B8G8R8X8_UNORM
    0,  // R10G10B10_XR_BIAS_A2_UNORM
    0,  // B8G8R8A8_TYPELESS
    32, // B8G8R8A8_UNORM_SRGB
    0,  // B8G8R8X8_TYPELESS
    32, // B8G8R8X8_UNORM_SRGB
    0,  // BC6H_TYPELESS
    0,  // BC6H_UF16
    0,  // BC6H_SF16
    0,  // BC7_TYPELESS
    0,  // BC7_UNORM
    0,  // BC7_UNORM_SRGB
    0,  // AYUV
    0,  // Y410
    0,  // Y416
    0,  // NV12
    0,  // P010
    0,  // P016
    0,  // YUV_420_OPAQUE
    0,  // YUY2
    0,  // Y210
    0,  // Y216
    0,  // NV11
    0,  // AI44
    0,  // IA44
    0,  // P8
    0,  // A8P8
    0,  // B4G4R4A4_UNORM
};

unsigned int s_pixel_data_bpp_custom[] = {
    24, // R8G8B8_UNORM
    32, // R8G8B8X8_UNORM
};
} // namespace

bool is_compressed_format(ImageFormat format)
{
    auto format_value = Util::to_underlying(format);
    return Util::to_underlying(ImageFormat::BC1_TYPELESS) <= format_value &&
               format_value <= Util::to_underlying(ImageFormat::BC5_SNORM) ||
           Util::to_underlying(ImageFormat::BC6H_TYPELESS) <= format_value &&
               format_value <= Util::to_underlying(ImageFormat::BC7_UNORM_SRGB);
}

unsigned int get_pixel_data_bpp(ImageFormat format)
{
    unsigned int bpp{};

    auto format_index = Util::to_underlying(format);
    if (format_index < Util::to_underlying(ImageFormat::FIRST_CUSTOM_FORMAT))
    {
        if (format_index < std::size(s_pixel_data_bpp_dxgi))
            bpp = s_pixel_data_bpp_dxgi[format_index];
    }
    else
    {
        auto offset = format_index - Util::to_underlying(ImageFormat::FIRST_CUSTOM_FORMAT);
        if (offset < std::size(s_pixel_data_bpp_custom))
            bpp = s_pixel_data_bpp_custom[offset];
    }

    LOG_ERROR_IF(bpp == 0, "no pixel data bpp defined for image format: {}", format_index);
    return bpp;
}

void calc_pixel_data_pitch(
    ImageFormat image_format, std::uint32_t width, std::uint32_t height, unsigned int &pitch, unsigned int &row_count
)
{
    switch (image_format)
    {
    case ImageFormat::BC1_TYPELESS:
    case ImageFormat::BC1_UNORM:
    case ImageFormat::BC1_UNORM_SRGB:
        pitch = std::max(1U, (width + 3) / 4) * 8;
        row_count = (height + 3) / 4;
        break;
    case ImageFormat::BC2_TYPELESS:
    case ImageFormat::BC2_UNORM:
    case ImageFormat::BC2_UNORM_SRGB:
    case ImageFormat::BC3_TYPELESS:
    case ImageFormat::BC3_UNORM:
    case ImageFormat::BC3_UNORM_SRGB:
        pitch = std::max(1U, (width + 3) / 4) * 16;
        row_count = (height + 3) / 4;
        break;
    case ImageFormat::R8G8_B8G8_UNORM:
    case ImageFormat::G8R8_G8B8_UNORM:
    case ImageFormat::YUY2:
    case ImageFormat::Y210:
    case ImageFormat::Y216:
        pitch = ((width + 1) >> 1) * 4;
        row_count = height;
        break;
    default:
        pitch = (width * get_pixel_data_bpp(image_format) + 7) / 8;
        row_count = height;
        break;
    }
}

unsigned int get_row_alignment_from_pitch(unsigned int pitch)
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
} // namespace Age::Gfx
