#include "common_texture.hpp"

namespace Age::Gfx
{
std::uint32_t get_base_image_pixel_data_size(const TextureDesc &texture_desc)
{
    unsigned int pitch, row_count;
    calc_pixel_data_pitch(texture_desc.format, texture_desc.width, texture_desc.height, pitch, row_count);

    return pitch * row_count * texture_desc.depth;
}

CubeMapFace CubeMap::operator[](std::size_t index) const
{
    if (index > 5)
    {
        Core::log_error("Cube map face index must be in the range [0, 5]: {}", index);
        index = 5;
    }
    return {
        .cube_map = *this,
        .byte_offset = this->face_size * static_cast<std::uint32_t>(index),
        .face_index = static_cast<std::uint32_t>(index)
    };
}

CubeMap get_cube_map(const TextureData &texture)
{
    return {.texture = texture, .face_size = get_mipmap_pixel_data_size(texture)};
}

bool operator==(const CubeMapFace &it1, const CubeMapFace &it2)
{
    return &it1.cube_map == &it2.cube_map && it1.face_index == it2.face_index;
}

bool operator!=(const CubeMapFace &it1, const CubeMapFace &it2)
{
    return !(it1 == it2);
}

CubeMapFace &operator++(CubeMapFace &it)
{
    it.byte_offset += it.cube_map.face_size;
    ++it.face_index;
    return it;
}

const CubeMapFace &operator*(const CubeMapFace &it)
{
    return it;
}

CubeMapFace begin(const CubeMap &cube_map)
{
    return {.cube_map = cube_map, .byte_offset = 0, .face_index = 0};
}

CubeMapFace end(const CubeMap &cube_map)
{
    return {.cube_map = cube_map, .face_index = 6};
}

Mipmap get_mipmap(const TextureData &texture)
{
    return {.texture = texture, .byte_offset = 0};
}

Mipmap get_mipmap(const CubeMapFace &cube_map_face)
{
    return {.texture = cube_map_face.cube_map.texture, .byte_offset = cube_map_face.byte_offset};
}

bool operator==(const MipmapLevel &it1, const MipmapLevel &it2)
{
    return &it1.texture == &it2.texture && it1.level == it2.level;
}

bool operator!=(const MipmapLevel &it1, const MipmapLevel &it2)
{
    return !(it1 == it2);
}

MipmapLevel &operator++(MipmapLevel &it)
{
    it.byte_offset += it.pitch * it.row_count * it.depth;
    ++it.level;

    it.width = std::max(1U, it.width >> 1);
    it.height = std::max(1U, it.height >> 1);
    it.depth = std::max(1U, it.depth >> 1);

    unsigned int pitch, row_count;
    calc_pixel_data_pitch(it.texture.desc.format, it.width, it.height, pitch, row_count);

    it.pitch = pitch;
    it.row_count = row_count;
    return it;
}

const MipmapLevel &operator*(const MipmapLevel &it)
{
    return it;
}

MipmapLevel begin(const Mipmap &mipmap)
{
    const TextureDesc &texture_desc{mipmap.texture.desc};

    unsigned int pitch, row_count;
    calc_pixel_data_pitch(texture_desc.format, texture_desc.width, texture_desc.height, pitch, row_count);

    return {
        .texture = mipmap.texture,
        .byte_offset = mipmap.byte_offset,
        .level = 0,
        .width = texture_desc.width,
        .height = texture_desc.height,
        .depth = texture_desc.depth,
        .pitch = pitch,
        .row_count = row_count
    };
}

MipmapLevel end(const Mipmap &mipmap)
{
    return {.texture = mipmap.texture, .level = mipmap.texture.desc.mipmap_level_count};
}

std::uint32_t get_mipmap_level_pixel_data_size(const MipmapLevel &mipmap_level)
{
    return mipmap_level.pitch * mipmap_level.row_count * mipmap_level.depth;
}

std::uint32_t get_mipmap_pixel_data_size(const TextureData &texture)
{
    std::uint32_t mipmap_size{0};
    for (const auto &mipmap_level : get_mipmap(texture))
        mipmap_size += get_mipmap_level_pixel_data_size(mipmap_level);
    return mipmap_size;
}
} // namespace Age::Gfx
