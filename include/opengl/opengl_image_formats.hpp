#pragma once

#include "image_formats.hpp"
#include "opengl/opengl_api.hpp"

namespace Age::Gfx::OGL
{
GLint get_internal_format(ImageFormat format, bool force_srgb_internal_format);

struct PixelDataFormat
{
    GLenum format{};
    GLenum type{};
};

PixelDataFormat get_pixel_data_format(ImageFormat format);
} // namespace Age::Gfx::OGL
