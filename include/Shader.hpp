#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include "OpenGL.hpp"
#include "UniformBuffer.hpp"

namespace Age::Gfx
{
struct ShaderAsset
{
    OGL::ShaderType shader_type{};
    std::string_view file_path;
};

using ShaderId = std::uint32_t;

struct ShaderRenderState
{
    bool srgb_rendering : 1 {true};
};

struct ShaderCommonUniforms
{
    bool projection_block : 1 {true};
    bool lv_matrix : 1 {true};
    bool lv_normal_matrix : 1 {false};
};

struct Shader
{
    ShaderRenderState render_state{};
    GLuint shader_program{};
    UniformBlock projection_block{};
    GLint lv_matrix{-1};
    GLint lv_normal_matrix{-1};

    Shader(GLuint shader_program, ShaderCommonUniforms common_uniforms = {}, ShaderRenderState render_state = {});
};

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
}

Shader &get_shader(ShaderId shader_id);

void use_shader(const Shader &shader);
} // namespace Age::Gfx
