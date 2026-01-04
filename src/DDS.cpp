#include <algorithm>
#include <cstring>
#include <fstream>
#include <limits>
#include <utility>

#include "DDS.hpp"
#include "ErrorHandling.hpp"

namespace Age::Gfx
{
namespace
{
constexpr char DDS_MAGIC_NUMBER[] = "DDS ";

constexpr std::uint32_t DDPF_ALPHAPIXELS{0x1};
constexpr std::uint32_t DDPF_ALPHA{0x2};
constexpr std::uint32_t DDPF_FOURCC{0x4};
constexpr std::uint32_t DDPF_RGB{0x40};
constexpr std::uint32_t DDPF_YUV{0x200};
constexpr std::uint32_t DDPF_LUMINANCE{0x20000};

constexpr std::uint32_t DDS_DXT1{0x31545844};
constexpr std::uint32_t DDS_DXT2{0x32545844};
constexpr std::uint32_t DDS_DXT3{0x33545844};
constexpr std::uint32_t DDS_DXT4{0x34545844};
constexpr std::uint32_t DDS_DXT5{0x35545844};
constexpr std::uint32_t DDS_DX10{0x30315844};

struct DDS_PIXELFORMAT
{
    std::uint32_t size;
    std::uint32_t flags;
    std::uint32_t fourCC;
    std::uint32_t rgbBitCount;
    std::uint32_t rBitMask;
    std::uint32_t gBitMask;
    std::uint32_t bBitMask;
    std::uint32_t aBitMask;
};

constexpr std::uint32_t DDSCAPS_COMPLEX{0x8};
constexpr std::uint32_t DDSCAPS_MIPMAP{0x400000};
constexpr std::uint32_t DDSCAPS_TEXTURE{0x1000};

constexpr std::uint32_t DDSCAPS2_CUBEMAP{0x200};
constexpr std::uint32_t DDS_CUBEMAP_ALLFACES{0xfc00};
constexpr std::uint32_t DDSCAPS2_VOLUME{0x200000};

struct DDS_HEADER
{
    std::uint32_t size;
    std::uint32_t flags;
    std::uint32_t height;
    std::uint32_t width;
    std::uint32_t pitchOrLinearSize;
    std::uint32_t depth;
    std::uint32_t mipMapCount;
    std::uint32_t reserved1[11];
    DDS_PIXELFORMAT ddspf;
    std::uint32_t caps;
    std::uint32_t caps2;
    std::uint32_t caps3;
    std::uint32_t caps4;
    std::uint32_t reserved2;
};

enum DXGI_FORMAT : std::uint32_t
{
    DXGI_FORMAT_UNKNOWN = 0,
    DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
    DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
    DXGI_FORMAT_R32G32B32A32_UINT = 3,
    DXGI_FORMAT_R32G32B32A32_SINT = 4,
    DXGI_FORMAT_R32G32B32_TYPELESS = 5,
    DXGI_FORMAT_R32G32B32_FLOAT = 6,
    DXGI_FORMAT_R32G32B32_UINT = 7,
    DXGI_FORMAT_R32G32B32_SINT = 8,
    DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
    DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
    DXGI_FORMAT_R16G16B16A16_UNORM = 11,
    DXGI_FORMAT_R16G16B16A16_UINT = 12,
    DXGI_FORMAT_R16G16B16A16_SNORM = 13,
    DXGI_FORMAT_R16G16B16A16_SINT = 14,
    DXGI_FORMAT_R32G32_TYPELESS = 15,
    DXGI_FORMAT_R32G32_FLOAT = 16,
    DXGI_FORMAT_R32G32_UINT = 17,
    DXGI_FORMAT_R32G32_SINT = 18,
    DXGI_FORMAT_R32G8X24_TYPELESS = 19,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
    DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
    DXGI_FORMAT_R10G10B10A2_UNORM = 24,
    DXGI_FORMAT_R10G10B10A2_UINT = 25,
    DXGI_FORMAT_R11G11B10_FLOAT = 26,
    DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
    DXGI_FORMAT_R8G8B8A8_UNORM = 28,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
    DXGI_FORMAT_R8G8B8A8_UINT = 30,
    DXGI_FORMAT_R8G8B8A8_SNORM = 31,
    DXGI_FORMAT_R8G8B8A8_SINT = 32,
    DXGI_FORMAT_R16G16_TYPELESS = 33,
    DXGI_FORMAT_R16G16_FLOAT = 34,
    DXGI_FORMAT_R16G16_UNORM = 35,
    DXGI_FORMAT_R16G16_UINT = 36,
    DXGI_FORMAT_R16G16_SNORM = 37,
    DXGI_FORMAT_R16G16_SINT = 38,
    DXGI_FORMAT_R32_TYPELESS = 39,
    DXGI_FORMAT_D32_FLOAT = 40,
    DXGI_FORMAT_R32_FLOAT = 41,
    DXGI_FORMAT_R32_UINT = 42,
    DXGI_FORMAT_R32_SINT = 43,
    DXGI_FORMAT_R24G8_TYPELESS = 44,
    DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
    DXGI_FORMAT_R8G8_TYPELESS = 48,
    DXGI_FORMAT_R8G8_UNORM = 49,
    DXGI_FORMAT_R8G8_UINT = 50,
    DXGI_FORMAT_R8G8_SNORM = 51,
    DXGI_FORMAT_R8G8_SINT = 52,
    DXGI_FORMAT_R16_TYPELESS = 53,
    DXGI_FORMAT_R16_FLOAT = 54,
    DXGI_FORMAT_D16_UNORM = 55,
    DXGI_FORMAT_R16_UNORM = 56,
    DXGI_FORMAT_R16_UINT = 57,
    DXGI_FORMAT_R16_SNORM = 58,
    DXGI_FORMAT_R16_SINT = 59,
    DXGI_FORMAT_R8_TYPELESS = 60,
    DXGI_FORMAT_R8_UNORM = 61,
    DXGI_FORMAT_R8_UINT = 62,
    DXGI_FORMAT_R8_SNORM = 63,
    DXGI_FORMAT_R8_SINT = 64,
    DXGI_FORMAT_A8_UNORM = 65,
    DXGI_FORMAT_R1_UNORM = 66,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
    DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
    DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
    DXGI_FORMAT_BC1_TYPELESS = 70,
    DXGI_FORMAT_BC1_UNORM = 71,
    DXGI_FORMAT_BC1_UNORM_SRGB = 72,
    DXGI_FORMAT_BC2_TYPELESS = 73,
    DXGI_FORMAT_BC2_UNORM = 74,
    DXGI_FORMAT_BC2_UNORM_SRGB = 75,
    DXGI_FORMAT_BC3_TYPELESS = 76,
    DXGI_FORMAT_BC3_UNORM = 77,
    DXGI_FORMAT_BC3_UNORM_SRGB = 78,
    DXGI_FORMAT_BC4_TYPELESS = 79,
    DXGI_FORMAT_BC4_UNORM = 80,
    DXGI_FORMAT_BC4_SNORM = 81,
    DXGI_FORMAT_BC5_TYPELESS = 82,
    DXGI_FORMAT_BC5_UNORM = 83,
    DXGI_FORMAT_BC5_SNORM = 84,
    DXGI_FORMAT_B5G6R5_UNORM = 85,
    DXGI_FORMAT_B5G5R5A1_UNORM = 86,
    DXGI_FORMAT_B8G8R8A8_UNORM = 87,
    DXGI_FORMAT_B8G8R8X8_UNORM = 88,
    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
    DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
    DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
    DXGI_FORMAT_BC6H_TYPELESS = 94,
    DXGI_FORMAT_BC6H_UF16 = 95,
    DXGI_FORMAT_BC6H_SF16 = 96,
    DXGI_FORMAT_BC7_TYPELESS = 97,
    DXGI_FORMAT_BC7_UNORM = 98,
    DXGI_FORMAT_BC7_UNORM_SRGB = 99,
    DXGI_FORMAT_AYUV = 100,
    DXGI_FORMAT_Y410 = 101,
    DXGI_FORMAT_Y416 = 102,
    DXGI_FORMAT_NV12 = 103,
    DXGI_FORMAT_P010 = 104,
    DXGI_FORMAT_P016 = 105,
    DXGI_FORMAT_420_OPAQUE = 106,
    DXGI_FORMAT_YUY2 = 107,
    DXGI_FORMAT_Y210 = 108,
    DXGI_FORMAT_Y216 = 109,
    DXGI_FORMAT_NV11 = 110,
    DXGI_FORMAT_AI44 = 111,
    DXGI_FORMAT_IA44 = 112,
    DXGI_FORMAT_P8 = 113,
    DXGI_FORMAT_A8P8 = 114,
    DXGI_FORMAT_B4G4R4A4_UNORM = 115,
    DXGI_FORMAT_P208 = 130,
    DXGI_FORMAT_V208 = 131,
    DXGI_FORMAT_V408 = 132,
    DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
    DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
};

enum D3D10_RESOURCE_DIMENSION : std::uint32_t
{
    D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
    D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
    D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4,
};

struct DDS_HEADER_DXT10
{
    DXGI_FORMAT dxgiFormat;
    D3D10_RESOURCE_DIMENSION resourceDimension;
    std::uint32_t miscFlag;
    std::uint32_t arraySize;
    std::uint32_t miscFlags2;
};

// TODO: handle endianness
// TODO: handle invalid input

bool is_extended_header_format(const DDS_HEADER &header)
{
    return header.ddspf.flags & DDPF_FOURCC && header.ddspf.fourCC == DDS_DX10;
}

unsigned int get_bit_count_per_pixel(const DDS_HEADER &header, const DDS_HEADER_DXT10 &header_extension)
{
    if (is_extended_header_format(header) == false)
        return header.ddspf.rgbBitCount;

    switch (header_extension.dxgiFormat)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return 32;
    case DXGI_FORMAT_R8_UNORM:
        return 8;
    default:
        return 0;
    }
}

bool read_dds_header(
    std::ifstream &fstream,
    DDS_HEADER &header,
    DDS_HEADER_DXT10 &header_extension,
    std::size_t &texture_size,
    std::string_view file_path
)
{
    fstream.seekg(0, std::ios_base::end);
    VBAIL_ERROR_IF(fstream.fail(), false, "IO error: {}", file_path);

    auto file_size = static_cast<std::size_t>(fstream.tellg());
    VBAIL_ERROR_IF(fstream.fail(), false, "IO error: {}", file_path);

    fstream.seekg(0);
    VBAIL_ERROR_IF(fstream.fail(), false, "IO error: {}", file_path);

    bool is_file_size_valid = file_size >= sizeof(DDS_MAGIC_NUMBER) - 1 + sizeof(DDS_HEADER);
    VBAIL_ERROR_IF(!is_file_size_valid, false, "invalid file size: {}", file_path);

    file_size -= sizeof(DDS_MAGIC_NUMBER) - 1 + sizeof(DDS_HEADER);

    char magic_number[4];
    fstream.read(magic_number, sizeof(magic_number));
    VBAIL_ERROR_IF(fstream.fail(), false, "IO error: {}", file_path);

    bool is_dds_file = std::strncmp(magic_number, DDS_MAGIC_NUMBER, sizeof(magic_number)) == 0;
    VBAIL_ERROR_IF(!is_dds_file, false, "not a dds file: {}", file_path);

    fstream.read(reinterpret_cast<char *>(&header), sizeof(header));
    VBAIL_ERROR_IF(fstream.fail(), false, "IO error: {}", file_path);

    if (is_extended_header_format(header))
    {
        VBAIL_ERROR_IF(file_size < sizeof(DDS_HEADER_DXT10), false, "invalid file size: {}", file_path);

        file_size -= sizeof(DDS_HEADER_DXT10);

        fstream.read(reinterpret_cast<char *>(&header_extension), sizeof(header_extension));
        VBAIL_ERROR_IF(fstream.fail(), false, "IO error: {}", file_path);
    }

    texture_size = file_size;
    return true;
}

bool read_texture_bytes(
    std::ifstream &fstream, std::size_t texture_size, TextureData &texture_data, std::string_view file_path
)
{
    auto texture_bytes = std::make_unique_for_overwrite<std::byte[]>(texture_size);
    fstream.read(reinterpret_cast<char *>(texture_bytes.get()), texture_size);
    VBAIL_ERROR_IF(fstream.fail(), false, "IO error: {}", file_path);

    texture_data.bytes = std::move(texture_bytes);
    return true;
}

bool detect_texture_format(
    const DDS_HEADER &header,
    const DDS_HEADER_DXT10 &header_extension,
    TextureDesc &texture_desc,
    std::string_view file_path
)
{
    // TODO: detect other formats when needs come
    if (header.ddspf.flags & DDPF_FOURCC) // block compressed or custom texture format
    {
        switch (header.ddspf.fourCC)
        {
        case DDS_DXT1:
            texture_desc.format = TextureFormat::BC1_UNORM;
            break;
        case DDS_DXT2:
            texture_desc.format = TextureFormat::BC2_UNORM;
            texture_desc.alpha_type = AlphaType::PREMULTIPLIED;
            break;
        case DDS_DXT3:
            texture_desc.format = TextureFormat::BC2_UNORM;
            texture_desc.alpha_type = AlphaType::STRAIGHT;
            break;
        case DDS_DXT4:
            texture_desc.format = TextureFormat::BC3_UNORM;
            texture_desc.alpha_type = AlphaType::PREMULTIPLIED;
            break;
        case DDS_DXT5:
            texture_desc.format = TextureFormat::BC3_UNORM;
            texture_desc.alpha_type = AlphaType::STRAIGHT;
            break;
        case DDS_DX10:
            texture_desc.format = static_cast<TextureFormat>(header_extension.dxgiFormat);
            break;
        }
    }
    else
    {
        if (header.ddspf.flags & DDPF_RGB) // r, g, b
        {
            if (header.ddspf.flags & DDPF_ALPHAPIXELS) // a
            {
                if (header.ddspf.rgbBitCount == 32)
                {
                    if (header.ddspf.rBitMask == 0x00ff0000 && header.ddspf.gBitMask == 0x0000ff00 &&
                        header.ddspf.bBitMask == 0x000000ff && header.ddspf.aBitMask == 0xff000000)
                    {
                        texture_desc.format = TextureFormat::B8G8R8A8_UNORM;
                        texture_desc.alpha_type = AlphaType::STRAIGHT;
                    }
                    else if (header.ddspf.rBitMask == 0x000000ff && header.ddspf.gBitMask == 0x0000ff00 &&
                             header.ddspf.bBitMask == 0x00ff0000 && header.ddspf.aBitMask == 0xff000000)
                    {
                        texture_desc.format = TextureFormat::R8G8B8A8_UNORM;
                        texture_desc.alpha_type = AlphaType::STRAIGHT;
                    }
                }
            }
            else
            {
                if (header.ddspf.rgbBitCount == 32)
                {
                    if (header.ddspf.rBitMask == 0x00ff0000 && header.ddspf.gBitMask == 0x0000ff00 &&
                        header.ddspf.bBitMask == 0x000000ff)
                    {
                        texture_desc.format = TextureFormat::B8G8R8X8_UNORM;
                        texture_desc.alpha_type = AlphaType::STRAIGHT;
                    }
                    else if (header.ddspf.rBitMask == 0x000000ff && header.ddspf.gBitMask == 0x0000ff00 &&
                             header.ddspf.bBitMask == 0x00ff0000)
                    {
                        texture_desc.format = TextureFormat::R8G8B8X8_UNORM;
                        texture_desc.alpha_type = AlphaType::STRAIGHT;
                    }
                }
            }
        }
        else if (header.ddspf.flags & DDPF_YUV) // r (Y), g (U), b (V)
        {
            if (header.ddspf.flags & DDPF_ALPHAPIXELS) // a
            {
            }
            else
            {
            }
        }
        else if (header.ddspf.flags & DDPF_LUMINANCE) // r
        {
            if (header.ddspf.flags & DDPF_ALPHAPIXELS) // a
            {
            }
            else
            {
                if (header.ddspf.rgbBitCount == 8)
                {
                    if (header.ddspf.rBitMask == 0xff)
                        texture_desc.format = TextureFormat::R8_UNORM;
                }
            }
        }
        else if (header.ddspf.flags & DDPF_ALPHA) // a
        {
        }
    }

    VBAIL_ERROR_IF(texture_desc.format == TextureFormat::UNKNOWN, false, "unrecognized texture format: {}", file_path);
    return true;
}

bool detect_texture_dimensions(const DDS_HEADER &header, TextureDesc &texture_desc, std::string_view file_path)
{
    if (header.caps2 & DDSCAPS2_VOLUME) // volume texture
    {
        texture_desc.type = TextureType::TEXTURE_3D;
        texture_desc.width = header.width;
        texture_desc.height = header.height;
        texture_desc.depth = header.depth;
    }
    else if (header.caps2 & DDSCAPS2_CUBEMAP) // cubemap texture
    {
        bool is_full_cubemap = (header.caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES;
        VBAIL_ERROR_IF(!is_full_cubemap, false, "partial cubemap textures are unsupported: {}", file_path);

        texture_desc.type = TextureType::TEXTURE_2D;
        texture_desc.width = header.width;
        texture_desc.height = header.height;
        texture_desc.depth = 1;
    }
    else if (header.height > 1) // 2d texture
    {
        texture_desc.type = TextureType::TEXTURE_2D;
        texture_desc.width = header.width;
        texture_desc.height = header.height;
        texture_desc.depth = 1;
    }
    else // 1d texture
    {
        texture_desc.type = TextureType::TEXTURE_1D;
        texture_desc.width = header.width;
        texture_desc.height = 1;
        texture_desc.depth = 1;
    }

    return true;
}

void detect_texture_count(const DDS_HEADER &header, const DDS_HEADER_DXT10 &header_extension, TextureDesc &texture_desc)
{
    if (header.caps & DDSCAPS_MIPMAP) // mipmap texture
    {
        texture_desc.mipmap_level_count = header.mipMapCount;
    }
    else
    {
        texture_desc.mipmap_level_count = 1;
    }

    if (is_extended_header_format(header) && texture_desc.type != TextureType::TEXTURE_3D)
    {
        texture_desc.count = header_extension.arraySize;
    }
    else
    {
        texture_desc.count = 1;
    }
}

void calc_texture_pitch(const DDS_HEADER &header, const DDS_HEADER_DXT10 &header_extension, TextureData &texture_data)
{
    // TODO: handle block compressed formats
    switch (texture_data.desc.format)
    {
    case TextureFormat::BC1_TYPELESS:
    case TextureFormat::BC1_UNORM:
    case TextureFormat::BC1_UNORM_SRGB:
        texture_data.row_pitch = std::max(1U, (header.width + 3) / 4) * 8;
        texture_data.row_count = (header.height + 3) / 4;
        break;
    case TextureFormat::BC2_TYPELESS:
    case TextureFormat::BC2_UNORM:
    case TextureFormat::BC2_UNORM_SRGB:
    case TextureFormat::BC3_TYPELESS:
    case TextureFormat::BC3_UNORM:
    case TextureFormat::BC3_UNORM_SRGB:
        texture_data.row_pitch = std::max(1U, (header.width + 3) / 4) * 16;
        texture_data.row_count = (header.height + 3) / 4;
        break;
    case TextureFormat::R8G8_B8G8_UNORM:
    case TextureFormat::G8R8_G8B8_UNORM:
    case TextureFormat::YUY2:
    case TextureFormat::Y210:
    case TextureFormat::Y216:
        texture_data.row_pitch = ((header.width + 1) >> 1) * 4;
        texture_data.row_count = header.height;
        break;
    default:
        texture_data.row_pitch = (header.width * get_bit_count_per_pixel(header, header_extension) + 7) / 8;
        texture_data.row_count = header.height;
        break;
    }
}
} // namespace

bool read_texture_data_from_dds_file(std::string_view file_path, TextureData &texture_data)
{
    std::ifstream fstream{file_path.data(), std::ios_base::binary};
    VBAIL_ERROR_IF(fstream.fail(), false, "failed to open file: {}", file_path);

    DDS_HEADER dds_header;
    DDS_HEADER_DXT10 dds_header_extension;
    std::size_t texture_size;

    if (!read_dds_header(fstream, dds_header, dds_header_extension, texture_size, file_path))
        return false;

    if (!detect_texture_format(dds_header, dds_header_extension, texture_data.desc, file_path))
        return false;

    if (!detect_texture_dimensions(dds_header, texture_data.desc, file_path))
        return false;

    detect_texture_count(dds_header, dds_header_extension, texture_data.desc);

    calc_texture_pitch(dds_header, dds_header_extension, texture_data);

    return read_texture_bytes(fstream, texture_size, texture_data, file_path);
}
} // namespace Age::Gfx
