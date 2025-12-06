#pragma once

#include <string_view>

#include "Texture.hpp"

namespace Age::Gfx
{
bool read_texture_data_from_dds_file(std::string_view file_path, TextureData &texture_data);
} // namespace Age::Gfx
