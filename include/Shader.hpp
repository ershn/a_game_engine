#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include "Matrix.hpp"
#include "OpenGL.hpp"

namespace Age::Gfx
{
struct ShaderAsset
{
    OGL::ShaderType shader_type{};
    std::string_view file_path;
};

using ShaderId = std::uint32_t;

inline constexpr unsigned int SHADER_LV_MATRIX{0b1};
inline constexpr unsigned int SHADER_LV_NORMAL_MATRIX{0b10};
inline constexpr unsigned int SHADER_LIGHT_DATA_BLOCK{0b100};

struct Shader
{
    GLuint shader_program{};
    GLint local_to_view_matrix{-1};
    GLint local_to_view_normal_matrix{-1};
    GLuint gamma_correction_block{GL_INVALID_INDEX};
    GLuint projection_block{GL_INVALID_INDEX};
    GLuint light_data_block{GL_INVALID_INDEX};

    Shader(GLuint shader_program, unsigned int options = 0U);
};

enum BlockBinding : GLuint
{
    GAMMA_CORRECTION_BLOCK_BINDING,
    PROJECTION_BLOCK_BINDING,
    LIGHT_DATA_BLOCK_BINDING
};

inline constexpr GLuint USER_BLOCK_BINDING_START_ID{32};

extern std::vector<std::unique_ptr<Shader>> g_shaders;

void init_shader_system();

GLuint create_shader_program(std::span<const ShaderAsset> shader_assets);

template <typename TShader>
void create_shader(ShaderId shader_id, std::span<const ShaderAsset> shader_assets)
{
    if (shader_id >= g_shaders.size())
        g_shaders.resize(shader_id + 1);

    GLuint shader_program{create_shader_program(shader_assets)};
    g_shaders[shader_id] = std::make_unique<TShader>(shader_program);

    const Shader &shader{*g_shaders[shader_id]};
    if (shader.gamma_correction_block != GL_INVALID_INDEX)
        OGL::bind_uniform_block(shader_program, shader.gamma_correction_block, GAMMA_CORRECTION_BLOCK_BINDING);
    if (shader.projection_block != GL_INVALID_INDEX)
        OGL::bind_uniform_block(shader_program, shader.projection_block, PROJECTION_BLOCK_BINDING);
    if (shader.light_data_block != GL_INVALID_INDEX)
        OGL::bind_uniform_block(shader_program, shader.light_data_block, LIGHT_DATA_BLOCK_BINDING);
}

const Shader &get_shader(ShaderId shader_id);
} // namespace Age::Gfx
