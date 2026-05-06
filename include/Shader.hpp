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

using ShaderId = std::uint16_t;

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

struct DrawQueue
{
    enum BitWidth
    {
        QUEUE = 11,
        TOTAL = 12,
    };

    unsigned short transparent : 1 {false};
    unsigned short queue : BitWidth::QUEUE{1000};

    static constexpr DrawQueue from(unsigned short draw_queue)
    {
        return {
            .transparent = static_cast<unsigned short>(draw_queue >> BitWidth::QUEUE & 0b1),
            .queue = static_cast<unsigned short>(draw_queue & (1 << BitWidth::QUEUE) - 1)
        };
    }

    constexpr operator unsigned short() const
    {
        return transparent << BitWidth::QUEUE | queue;
    }

    static const DrawQueue min_opaque;
    static const DrawQueue max_opaque;
    static const DrawQueue min_transparent;
    static const DrawQueue max_transparent;
    static const DrawQueue min;
    static const DrawQueue max;
};

inline constexpr DrawQueue DrawQueue::min_opaque{.transparent = false, .queue = 0};
inline constexpr DrawQueue DrawQueue::max_opaque{.transparent = false, .queue = (1 << BitWidth::QUEUE) - 1};
inline constexpr DrawQueue DrawQueue::min_transparent{.transparent = true, .queue = 0};
inline constexpr DrawQueue DrawQueue::max_transparent{.transparent = true, .queue = (1 << BitWidth::QUEUE) - 1};
inline constexpr DrawQueue DrawQueue::min{min_opaque};
inline constexpr DrawQueue DrawQueue::max{max_transparent};

struct Shader
{
    GLuint shader_program{};
    UniformBlock projection_block{};
    GLint lv_matrix{-1};
    GLint lv_normal_matrix{-1};
    DrawQueue draw_queue{};
    ShaderRenderState render_state{};

    Shader(
        GLuint shader_program,
        ShaderCommonUniforms common_uniforms = {},
        ShaderRenderState render_state = {},
        DrawQueue draw_queue = {}
    );
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
