#include "OpenGL.hpp"

#include "game/ShadersAndMaterials.hpp"

namespace Game
{
using namespace Age;

FragmentLightingShader::FragmentLightingShader(GLuint shader_program)
    : Shader{shader_program, {.lv_normal_matrix = true}}
    , light_block{Gfx::OGL::get_uniform_block_index(shader_program, "LightBlock")}
    , material_block{Gfx::OGL::get_uniform_block_index(shader_program, "MaterialBlock")}
    , gaussian_texture{Gfx::OGL::get_uniform_location(shader_program, "_gaussianTexture")}
{
}

FragmentLightingMaterial::FragmentLightingMaterial(Gfx::Shader &shader)
    : Material{shader}
{
}

void FragmentLightingMaterial::apply_properties() const
{
    auto &shader = static_cast<FragmentLightingShader &>(this->shader);
    Gfx::bind_uniform_buffer_range(shader.shader_program, shader.light_block, light_buffer_range_id);
    Gfx::bind_uniform_buffer_range(shader.shader_program, shader.material_block, material_buffer_range_id);
    Gfx::OGL::set_uniform(shader.gaussian_texture, gaussian_texture);
}

FragmentLightingColorShader::FragmentLightingColorShader(GLuint shader_program)
    : FragmentLightingShader{shader_program}
    , diffuse_color{Gfx::OGL::get_uniform_location(shader_program, "_diffuseColor")}
{
}

FragmentLightingColorMaterial::FragmentLightingColorMaterial(Gfx::Shader &shader)
    : FragmentLightingMaterial{shader}
{
}

void FragmentLightingColorMaterial::apply_properties() const
{
    FragmentLightingMaterial::apply_properties();
    auto &shader = static_cast<const FragmentLightingColorShader &>(this->shader);
    Gfx::OGL::set_uniform(shader.diffuse_color, diffuse_color);
}
} // namespace Game
