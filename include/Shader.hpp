#pragma once

#include <concepts>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include "DrawQueue.hpp"
#include "IdGenerator.hpp"
#include "OpenGL.hpp"
#include "RenderPipelineState.hpp"
#include "UniformBuffer.hpp"

namespace Age::Gfx
{
struct ShaderAsset
{
    OGL::ShaderType shader_type{};
    std::string_view file_path;
};

enum struct ShaderId : std::uint32_t
{
};

constexpr std::size_t to_index(ShaderId id)
{
    return static_cast<std::size_t>(id);
}

struct ShaderCommonUniforms
{
    bool projection_block : 1 {true};
    bool lv_matrix : 1 {true};
    bool lv_normal_matrix : 1 {false};
};

struct Shader
{
    const GLuint shader_program{};
    UniformBlock projection_block{};
    const GLint lv_matrix{-1};
    const GLint lv_normal_matrix{-1};
    const RenderPipelineState render_state{};
    const DrawQueue draw_queue{};

    Shader(
        GLuint shader_program,
        ShaderCommonUniforms common_uniforms = {},
        RenderPipelineState render_state = {},
        DrawQueue draw_queue = {}
    );
};

template <std::derived_from<Shader> TShader>
struct IdentifiedShader
{
    ShaderId id{};
    TShader &shader;
};

extern Util::IdGenerator<ShaderId> g_shader_id_generator;
extern std::vector<std::unique_ptr<Shader>> g_shaders;

void init_shader_system();

GLuint create_shader_program(std::span<const ShaderAsset> shader_assets);

template <std::derived_from<Shader> TShader>
IdentifiedShader<TShader> create_shader(std::span<const ShaderAsset> shader_assets)
{
    ShaderId shader_id{g_shader_id_generator.generate()};
    std::size_t shader_index{to_index(shader_id)};

    if (shader_index >= g_shaders.size())
        g_shaders.resize(shader_index + 1);

    GLuint shader_program{create_shader_program(shader_assets)};
    g_shaders[shader_index] = std::make_unique<TShader>(shader_program);
    return {shader_id, static_cast<TShader &>(*g_shaders[shader_index])};
}

template <std::derived_from<Shader> TShader>
IdentifiedShader<TShader> create_shader(std::initializer_list<ShaderAsset> shader_assets)
{
    return create_shader<TShader>(std::span{shader_assets.begin(), shader_assets.end()});
}

Shader &get_shader(ShaderId shader_id);

void use_shader(const Shader &shader);
} // namespace Age::Gfx
