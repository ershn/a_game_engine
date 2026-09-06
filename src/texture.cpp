#include <algorithm>
#include <vector>

#include "error_handling.hpp"
#include "id_generator.hpp"
#include "opengl/opengl_image_formats.hpp"
#include "texture.hpp"
#include "utils.hpp"

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
constexpr GLenum s_texture_type_to_gl_enum[] = {GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_3D};

constexpr GLenum to_gl_enum(TextureType texture_type)
{
    return s_texture_type_to_gl_enum[static_cast<std::size_t>(texture_type)];
}

constexpr GLenum s_texture_wrap_mode_to_gl_enum[] = {
    GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
};

constexpr GLenum to_gl_enum(TextureWrapMode texture_wrap_mode)
{
    return s_texture_wrap_mode_to_gl_enum[static_cast<std::size_t>(texture_wrap_mode)];
}

constexpr GLenum s_texture_min_filter_to_gl_enum[] = {
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

constexpr GLenum s_texture_mag_filter_to_gl_enum[] = {GL_NEAREST, GL_LINEAR};

constexpr GLenum to_gl_enum(TextureMagFilter texture_mag_filter)
{
    return s_texture_mag_filter_to_gl_enum[static_cast<std::size_t>(texture_mag_filter)];
}

std::uint32_t get_base_image_pixel_data_size(const TextureDesc &texture_desc)
{
    unsigned int pitch, row_count;
    calc_pixel_data_pitch(texture_desc.format, texture_desc.width, texture_desc.height, pitch, row_count);

    return pitch * row_count * texture_desc.depth;
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
} // namespace

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

struct Texture
{
    GLuint texture{};
    TextureUnitId bound_texture_unit_id{NULL_TEXTURE_UNIT_ID};
    TextureType type{};
};

struct Sampler
{
    GLuint sampler{};
    TextureUnitId bound_texture_unit_id{NULL_TEXTURE_UNIT_ID};
};

namespace
{
struct TextureUnit
{
    TextureId bound_texture_id{};
    std::uint32_t sampler_use_count{};
    SamplerId bound_sampler_id{};
};

constexpr auto MAX_RENDER_TARGET_ID{std::numeric_limits<std::underlying_type_t<RenderTargetId>>::max()};
constexpr TextureId FIRST_TEXTURE_ID{1};
constexpr TextureId LAST_TEXTURE_ID{MAX_RENDER_TARGET_ID / 2};

constexpr std::size_t to_index(TextureId id)
{
    return static_cast<std::size_t>(id) - static_cast<std::size_t>(FIRST_TEXTURE_ID);
}

constexpr std::size_t to_index(SamplerId id)
{
    return static_cast<std::size_t>(id) - 1;
}

std::vector<TextureUnit> s_texture_units;

Util::IdGenerator<TextureId> s_texture_id_generator{FIRST_TEXTURE_ID};
std::vector<Texture> s_textures;
std::vector<TextureDesc> s_texture_descs;

Util::IdGenerator<SamplerId> s_sampler_id_generator{SamplerId{1}};
std::vector<Sampler> s_samplers;
std::vector<SamplerParams> s_sampler_params;

void unbind_texture(TextureUnit &texture_unit);

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

        if (least_used_tex_unit->bound_texture_id == TextureId{} &&
                (tex_unit.bound_texture_id == TextureId{} &&
                 tex_unit.sampler_use_count < least_used_tex_unit->sampler_use_count) ||
            least_used_tex_unit->bound_texture_id != TextureId{} &&
                (tex_unit.bound_texture_id == TextureId{} ||
                 tex_unit.sampler_use_count < least_used_tex_unit->sampler_use_count))
        {
            least_used_tex_unit_id = tex_unit_id;
            least_used_tex_unit = &tex_unit;
        }
    }

    return {least_used_tex_unit_id, *least_used_tex_unit};
}

void select_temporary_texture_unit()
{
    auto [texture_unit_id, texture_unit] = get_least_used_texture_unit();
    unbind_texture(texture_unit);
    glActiveTexture(GL_TEXTURE0 + texture_unit_id);
}

GLint get_pixel_data_unpack_alignment()
{
    return OGL::get_integer(GL_UNPACK_ALIGNMENT);
}

void update_pixel_data_unpack_alignment(unsigned int pitch, GLint &unpack_alignment)
{
    GLint row_alignment{static_cast<GLint>(get_row_alignment_from_pitch(pitch))};
    if (row_alignment != unpack_alignment)
    {
        unpack_alignment = row_alignment;
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_alignment);
    }
}

using TextureImageLoader = void (*)(
    std::uint32_t level,
    GLint internal_format,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    OGL::PixelDataFormat data_format,
    const std::byte *bytes
);

void load_1d_texture_image(
    std::uint32_t level,
    GLint internal_format,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    OGL::PixelDataFormat data_format,
    const std::byte *bytes
)
{
    glTexImage1D(
        GL_TEXTURE_1D,
        static_cast<GLint>(level),
        internal_format,
        static_cast<GLsizei>(width),
        0,
        data_format.format,
        data_format.type,
        bytes
    );
}

void load_2d_texture_image(
    std::uint32_t level,
    GLint internal_format,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    OGL::PixelDataFormat data_format,
    const std::byte *bytes
)
{
    glTexImage2D(
        GL_TEXTURE_2D,
        static_cast<GLint>(level),
        internal_format,
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height),
        0,
        data_format.format,
        data_format.type,
        bytes
    );
}

void load_3d_texture_image(
    std::uint32_t level,
    GLint internal_format,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    OGL::PixelDataFormat data_format,
    const std::byte *bytes
)
{
    glTexImage3D(
        GL_TEXTURE_3D,
        static_cast<GLint>(level),
        internal_format,
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height),
        static_cast<GLsizei>(depth),
        0,
        data_format.format,
        data_format.type,
        bytes
    );
}

template <GLenum TextureTarget, TextureImageLoader LoadTextureImage>
GLuint create_texture_from_bytes(const TextureData &texture_data, TextureCreationOptions creation_options)
{
    const TextureDesc &texture_desc{texture_data.desc};

    auto internal_format = OGL::get_internal_format(texture_desc.format, creation_options.force_srgb_internal_format);
    auto data_format = OGL::get_pixel_data_format(texture_desc.format);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(TextureTarget, texture);

    GLint unpack_alignment{get_pixel_data_unpack_alignment()};

    if (texture_desc.mipmap_level_count > 1)
    {
        for (const auto &mipmap_level : get_mipmap(texture_data))
        {
            update_pixel_data_unpack_alignment(mipmap_level.pitch, unpack_alignment);

            LoadTextureImage(
                mipmap_level.level,
                internal_format,
                mipmap_level.width,
                mipmap_level.height,
                mipmap_level.depth,
                data_format,
                &mipmap_level.texture.bytes[mipmap_level.byte_offset]
            );
        }
    }
    else
    {
        unsigned int pitch, row_count;
        calc_pixel_data_pitch(texture_desc.format, texture_desc.width, texture_desc.height, pitch, row_count);
        update_pixel_data_unpack_alignment(pitch, unpack_alignment);

        LoadTextureImage(
            0,
            internal_format,
            texture_desc.width,
            texture_desc.height,
            texture_desc.depth,
            data_format,
            texture_data.bytes.get()
        );
    }

    glTexParameteri(TextureTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(TextureTarget, GL_TEXTURE_MAX_LEVEL, texture_desc.mipmap_level_count - 1);

    glBindTexture(TextureTarget, 0);

    return texture;
}

GLuint create_cube_map_texture_from_bytes(const TextureData &texture_data, TextureCreationOptions creation_options)
{
    const TextureDesc &texture_desc{texture_data.desc};

    auto internal_format = OGL::get_internal_format(texture_desc.format, creation_options.force_srgb_internal_format);
    auto data_format = OGL::get_pixel_data_format(texture_desc.format);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    GLint unpack_alignment{get_pixel_data_unpack_alignment()};

    if (texture_desc.mipmap_level_count > 1)
    {
        unsigned int face_index{0};
        for (const auto &cube_map_face : get_cube_map(texture_data))
        {
            for (const auto &mipmap_level : get_mipmap(cube_map_face))
            {
                update_pixel_data_unpack_alignment(mipmap_level.pitch, unpack_alignment);

                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_index,
                    mipmap_level.level,
                    internal_format,
                    mipmap_level.width,
                    mipmap_level.height,
                    0,
                    data_format.format,
                    data_format.type,
                    &mipmap_level.texture.bytes[mipmap_level.byte_offset]
                );
            }
            ++face_index;
        }
    }
    else
    {
        unsigned int pitch, row_count;
        calc_pixel_data_pitch(texture_desc.format, texture_desc.width, texture_desc.height, pitch, row_count);
        update_pixel_data_unpack_alignment(pitch, unpack_alignment);

        unsigned int face_index{0};
        for (const auto &cube_map_face : get_cube_map(texture_data))
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_index,
                0,
                internal_format,
                texture_desc.width,
                texture_desc.height,
                0,
                data_format.format,
                data_format.type,
                &cube_map_face.cube_map.texture.bytes[cube_map_face.byte_offset]
            );
            ++face_index;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, texture_desc.mipmap_level_count - 1);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

using CompressedTextureImageLoader = void (*)(
    std::uint32_t level,
    GLenum internal_format,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    std::uint32_t image_size,
    const std::byte *bytes
);

void load_compressed_1d_texture_image(
    std::uint32_t level,
    GLenum internal_format,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    std::uint32_t image_size,
    const std::byte *bytes
)
{
    glCompressedTexImage1D(
        GL_TEXTURE_1D,
        static_cast<GLint>(level),
        static_cast<GLenum>(internal_format),
        static_cast<GLsizei>(width),
        0,
        static_cast<GLsizei>(image_size),
        bytes
    );
}

void load_compressed_2d_texture_image(
    std::uint32_t level,
    GLenum internal_format,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    std::uint32_t image_size,
    const std::byte *bytes
)
{
    glCompressedTexImage2D(
        GL_TEXTURE_2D,
        static_cast<GLint>(level),
        static_cast<GLenum>(internal_format),
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height),
        0,
        static_cast<GLsizei>(image_size),
        bytes
    );
}

void load_compressed_3d_texture_image(
    std::uint32_t level,
    GLenum internal_format,
    std::uint32_t width,
    std::uint32_t height,
    std::uint32_t depth,
    std::uint32_t image_size,
    const std::byte *bytes
)
{
    glCompressedTexImage3D(
        GL_TEXTURE_3D,
        static_cast<GLint>(level),
        static_cast<GLenum>(internal_format),
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height),
        static_cast<GLsizei>(depth),
        0,
        static_cast<GLsizei>(image_size),
        bytes
    );
}

template <GLenum TextureTarget, CompressedTextureImageLoader LoadCompressedTextureImage>
GLuint create_compressed_texture_from_bytes(const TextureData &texture_data, TextureCreationOptions creation_options)
{
    const TextureDesc &texture_desc{texture_data.desc};

    auto internal_format = OGL::get_internal_format(texture_desc.format, creation_options.force_srgb_internal_format);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(TextureTarget, texture);

    if (texture_desc.mipmap_level_count > 1)
    {
        for (const auto &mipmap_level : get_mipmap(texture_data))
        {
            LoadCompressedTextureImage(
                mipmap_level.level,
                internal_format,
                mipmap_level.width,
                mipmap_level.height,
                mipmap_level.depth,
                get_mipmap_level_pixel_data_size(mipmap_level),
                &mipmap_level.texture.bytes[mipmap_level.byte_offset]
            );
        }
    }
    else
    {
        LoadCompressedTextureImage(
            0,
            internal_format,
            texture_desc.width,
            texture_desc.height,
            texture_desc.depth,
            get_base_image_pixel_data_size(texture_desc),
            texture_data.bytes.get()
        );
    }

    glTexParameteri(TextureTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(TextureTarget, GL_TEXTURE_MAX_LEVEL, texture_desc.mipmap_level_count - 1);

    glBindTexture(TextureTarget, 0);

    return texture;
}

GLuint create_compressed_cube_map_texture_from_bytes(
    const TextureData &texture_data, TextureCreationOptions creation_options
)
{
    const TextureDesc &texture_desc{texture_data.desc};

    auto internal_format = OGL::get_internal_format(texture_desc.format, creation_options.force_srgb_internal_format);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    if (texture_desc.mipmap_level_count > 1)
    {
        unsigned int face_index{0};
        for (const auto &cube_map_face : get_cube_map(texture_data))
        {
            for (const auto &mipmap_level : get_mipmap(cube_map_face))
            {
                glCompressedTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_index,
                    static_cast<GLint>(mipmap_level.level),
                    static_cast<GLenum>(internal_format),
                    static_cast<GLsizei>(mipmap_level.width),
                    static_cast<GLsizei>(mipmap_level.height),
                    0,
                    static_cast<GLsizei>(get_mipmap_level_pixel_data_size(mipmap_level)),
                    &mipmap_level.texture.bytes[mipmap_level.byte_offset]
                );
            }
            ++face_index;
        }
    }
    else
    {
        unsigned int face_index{0};
        for (const auto &cube_map_face : get_cube_map(texture_data))
        {
            glCompressedTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_index,
                0,
                static_cast<GLenum>(internal_format),
                static_cast<GLsizei>(texture_desc.width),
                static_cast<GLsizei>(texture_desc.height),
                0,
                static_cast<GLsizei>(get_base_image_pixel_data_size(texture_desc)),
                &cube_map_face.cube_map.texture.bytes[cube_map_face.byte_offset]
            );
            ++face_index;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, texture_desc.mipmap_level_count - 1);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

GLuint create_texture_from_bytes(const TextureData &texture_data, TextureCreationOptions creation_options)
{
    if (is_compressed_format(texture_data.desc.format))
    {
        switch (texture_data.desc.type)
        {
        case TextureType::TEXTURE_1D:
            return create_compressed_texture_from_bytes<GL_TEXTURE_1D, load_compressed_1d_texture_image>(
                texture_data, creation_options
            );
        case TextureType::TEXTURE_2D:
            return create_compressed_texture_from_bytes<GL_TEXTURE_2D, load_compressed_2d_texture_image>(
                texture_data, creation_options
            );
        case TextureType::TEXTURE_CUBE_MAP:
            return create_compressed_cube_map_texture_from_bytes(texture_data, creation_options);
        case TextureType::TEXTURE_3D:
            return create_compressed_texture_from_bytes<GL_TEXTURE_3D, load_compressed_3d_texture_image>(
                texture_data, creation_options
            );
        }
    }
    else
    {
        switch (texture_data.desc.type)
        {
        case TextureType::TEXTURE_1D:
            return create_texture_from_bytes<GL_TEXTURE_1D, load_1d_texture_image>(texture_data, creation_options);
        case TextureType::TEXTURE_2D:
            return create_texture_from_bytes<GL_TEXTURE_2D, load_2d_texture_image>(texture_data, creation_options);
        case TextureType::TEXTURE_CUBE_MAP:
            return create_cube_map_texture_from_bytes(texture_data, creation_options);
        case TextureType::TEXTURE_3D:
            return create_texture_from_bytes<GL_TEXTURE_3D, load_3d_texture_image>(texture_data, creation_options);
        }
    }

    Core::log_error("Unsupported texture type: {}", Util::to_underlying(texture_data.desc.type));
    return 0;
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
    if (texture_unit.bound_texture_id == TextureId{})
        return;

    s_textures[to_index(texture_unit.bound_texture_id)].bound_texture_unit_id = NULL_TEXTURE_UNIT_ID;
    texture_unit.bound_texture_id = TextureId{};
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
    .texture_border_color{0.0f},
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

    if (params.texture_border_color != prev_params.texture_border_color)
        glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, static_cast<const float *>(params.texture_border_color));

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
    if (texture_unit.bound_sampler_id == SamplerId{})
        return;

    s_samplers[to_index(texture_unit.bound_sampler_id)].bound_texture_unit_id = NULL_TEXTURE_UNIT_ID;
    texture_unit.bound_sampler_id = SamplerId{};
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

bool is_texture_id(RenderTargetId id)
{
    return Util::to_underlying(FIRST_TEXTURE_ID) <= Util::to_underlying(id) &&
           Util::to_underlying(id) <= Util::to_underlying(LAST_TEXTURE_ID);
}

RenderTargetId to_render_target_id(TextureId id)
{
    return static_cast<RenderTargetId>(id);
}

TextureId to_texture_id(RenderTargetId id)
{
    return static_cast<TextureId>(id);
}

TextureId create_texture(const TextureData &texture_data, TextureCreationOptions creation_options)
{
    TextureId texture_id{s_texture_id_generator.generate()};
    std::size_t texture_index{to_index(texture_id)};

    if (texture_index == s_textures.size())
    {
        s_textures.resize(texture_index + 1);
        s_texture_descs.resize(texture_index + 1);
    }

    s_texture_descs[texture_index] = texture_data.desc;

    Texture &texture{s_textures[texture_index]};
    texture.type = texture_data.desc.type;

    select_temporary_texture_unit();

    texture.texture = create_texture_from_bytes(texture_data, creation_options);

    return texture_id;
}

TextureId create_texture(TextureType texture_type, std::function<void(const Texture &, TextureDesc &)> specify_texture)
{
    TextureId texture_id{s_texture_id_generator.generate()};
    std::size_t texture_index{to_index(texture_id)};

    if (texture_index == s_textures.size())
    {
        s_textures.resize(texture_index + 1);
        s_texture_descs.resize(texture_index + 1);
    }

    Texture &texture{s_textures[texture_index]};
    TextureDesc &texture_desc{s_texture_descs[texture_index]};

    select_temporary_texture_unit();

    glGenTextures(1, &texture.texture);

    glBindTexture(to_gl_enum(texture_type), texture.texture);

    texture.type = texture_type;

    specify_texture(texture, texture_desc);

    glBindTexture(to_gl_enum(texture_type), 0);

    return texture_id;
}

void modify_texture(TextureId texture_id, std::function<void(const Texture &, TextureDesc &)> modify_texture)
{
    Texture &texture{s_textures[to_index(texture_id)]};
    TextureDesc &texture_desc{s_texture_descs[to_index(texture_id)]};

    select_temporary_texture_unit();

    glBindTexture(to_gl_enum(texture.type), texture.texture);

    modify_texture(texture, texture_desc);

    glBindTexture(to_gl_enum(texture.type), 0);
}

SamplerId create_sampler(const SamplerParams &sampler_params)
{
    SamplerId sampler_id{s_sampler_id_generator.generate()};
    std::size_t sampler_index{to_index(sampler_id)};

    if (sampler_index == s_samplers.size())
    {
        s_samplers.resize(sampler_index + 1);
        s_sampler_params.resize(sampler_index + 1);
    }

    Sampler &sampler{s_samplers[sampler_index]};
    glGenSamplers(1, &sampler.sampler);
    update_sampler_params(sampler.sampler, sampler_params, DEFAULT_SAMPLER_PARAMS);
    s_sampler_params[sampler_index] = sampler_params;

    return sampler_id;
}

const SamplerParams &get_sampler_params(SamplerId sampler_id)
{
    return s_sampler_params[to_index(sampler_id)];
}

void set_sampler_params(SamplerId sampler_id, const SamplerParams &sampler_params)
{
    const Sampler &sampler{s_samplers[to_index(sampler_id)]};
    SamplerParams &current_sampler_params{s_sampler_params[to_index(sampler_id)]};
    update_sampler_params(sampler.sampler, sampler_params, current_sampler_params);
    current_sampler_params = sampler_params;
}

void bind_texture_and_sampler(SamplerUniform &sampler_uniform, TextureId texture_id, SamplerId sampler_id)
{
    Texture &texture{s_textures[to_index(texture_id)]};
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

        Sampler &sampler{s_samplers[to_index(sampler_id)]};
        bind_sampler(sampler_id, sampler, texture_unit_id, *texture_unit);
    }
}
} // namespace Age::Gfx
