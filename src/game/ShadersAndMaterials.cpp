#include "OpenGL.hpp"

#include "game/ShadersAndMaterials.hpp"

namespace Game
{
using namespace Age;

FragmentLightingShader::FragmentLightingShader(GLuint shader_program)
    : Shader{shader_program, Gfx::SHADER_VIEW_MATRIX | Gfx::SHADER_VIEW_NORMAL_MATRIX | Gfx::SHADER_LIGHT_DATA_BLOCK}
    , material_block{Gfx::OGL::get_uniform_block_index(shader_program, "MaterialBlock")}
{
    Gfx::OGL::bind_uniform_block(shader_program, material_block, MATERIAL_BLOCK_BINDING);
}

FragmentLightingMaterial::FragmentLightingMaterial(const Gfx::Shader &shader)
    : Material{shader}
{
}

void FragmentLightingMaterial::apply_properties() const
{
}

FragmentLightingColorShader::FragmentLightingColorShader(GLuint shader_program)
    : FragmentLightingShader{shader_program}
    , diffuse_color{Gfx::OGL::get_uniform_location(shader_program, "uDiffuseColor")}
{
}

FragmentLightingColorMaterial::FragmentLightingColorMaterial(const Gfx::Shader &shader)
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
    , material_block{Gfx::OGL::get_uniform_block_index(shader_program, "MaterialBlock")}
    , sphere_view_position{Gfx::OGL::get_uniform_location(shader_program, "uSphereViewPosition")}
    , sphere_radius{Gfx::OGL::get_uniform_location(shader_program, "uSphereRadius")}
    , diffuse_color{Gfx::OGL::get_uniform_location(shader_program, "uDiffuseColor")}
{
}

SphereImpostorMaterial::SphereImpostorMaterial(const Age::Gfx::Shader &shader)
    : Material{shader}
{
}

void SphereImpostorMaterial::apply_properties() const
{
    auto &shader = static_cast<const SphereImpostorShader &>(this->shader);
    Gfx::OGL::set_uniform(shader.sphere_view_position, sphere_view_position);
    Gfx::OGL::set_uniform(shader.sphere_radius, sphere_radius);
    Gfx::OGL::set_uniform(shader.diffuse_color, diffuse_color);
}
} // namespace Game
