#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

#include "Matrix.hpp"
#include "OpenGL.hpp"

namespace Age::Gfx
{
using ShaderId = std::uint32_t;

inline constexpr unsigned int SHADER_CAMERA_NORMAL_MATRIX{0b1};
inline constexpr unsigned int SHADER_LIGHT_DATA_BLOCK{0b10};

struct Shader
{
    GLuint shader_program{};
    GLint model_to_camera_matrix{-1};
    GLint model_to_camera_normal_matrix{-1};
    GLuint projection_block{GL_INVALID_INDEX};
    GLuint light_data_block{GL_INVALID_INDEX};

    Shader(GLuint shader_program, unsigned int options = 0U);
};

enum BlockBinding : GLuint
{
    PROJECTION_BLOCK_BINDING,
    LIGHT_DATA_BLOCK_BINDING
};

extern std::vector<std::unique_ptr<Shader>> g_shaders;

void init_shader_system();

GLuint create_shader_program(std::string_view vertex_shader_path, std::string_view fragment_shader_path);

template <typename TShader>
void create_shader(ShaderId shader_id, std::string_view vertex_shader_path, std::string_view fragment_shader_path)
{
    if (shader_id >= g_shaders.size())
        g_shaders.resize(shader_id + 1);

    GLuint shader_program{create_shader_program(vertex_shader_path, fragment_shader_path)};
    g_shaders[shader_id] = std::make_unique<TShader>(shader_program);

    const Shader &shader{*g_shaders[shader_id]};
    bind_uniform_block(shader_program, shader.projection_block, PROJECTION_BLOCK_BINDING);
    if (shader.light_data_block != GL_INVALID_INDEX)
        bind_uniform_block(shader_program, shader.light_data_block, LIGHT_DATA_BLOCK_BINDING);
}

const Shader &get_shader(ShaderId shader_id);
} // namespace Age::Gfx
