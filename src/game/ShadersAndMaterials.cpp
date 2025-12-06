#include "OpenGL.hpp"

#include "game/ShadersAndMaterials.hpp"

namespace Game
{
using namespace Age;

FragmentLightingShader::FragmentLightingShader(GLuint shader_program)
    : Shader{shader_program, Gfx::SHADER_LV_MATRIX | Gfx::SHADER_LV_NORMAL_MATRIX | Gfx::SHADER_LIGHT_DATA_BLOCK}
    , gaussian_texture{Gfx::OGL::get_uniform_location(shader_program, "uGaussianTexture")}
    , material_block{Gfx::OGL::get_uniform_block_index(shader_program, "MaterialBlock")}
{
    Gfx::OGL::bind_uniform_block(shader_program, material_block, MATERIAL_BLOCK_BINDING);
}

FragmentLightingMaterial::FragmentLightingMaterial(Gfx::Shader &shader)
    : Material{shader}
{
}

void FragmentLightingMaterial::apply_properties() const
{
    auto &shader = static_cast<const FragmentLightingShader &>(this->shader);
    Gfx::OGL::set_uniform(shader.gaussian_texture, gaussian_texture);
}

FragmentLightingColorShader::FragmentLightingColorShader(GLuint shader_program)
    : FragmentLightingShader{shader_program}
    , diffuse_color{Gfx::OGL::get_uniform_location(shader_program, "uDiffuseColor")}
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

SphereImpostorShader::SphereImpostorShader(GLuint shader_program)
    : Shader{shader_program, Gfx::SHADER_LIGHT_DATA_BLOCK}
    , materials_block{Gfx::OGL::get_uniform_block_index(shader_program, "MaterialsBlock")}
{
    Gfx::OGL::bind_uniform_block(shader_program, materials_block, MATERIALS_BLOCK_BINDING);
}

SphereImpostorMaterial::SphereImpostorMaterial(Age::Gfx::Shader &shader)
    : Material{shader}
{
}

void SphereImpostorMaterial::apply_properties() const
{
}
} // namespace Game
