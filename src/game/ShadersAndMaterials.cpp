#include "OpenGL.hpp"

#include "game/ShadersAndMaterials.hpp"

namespace Game
{
using namespace Age;

FragmentLightingShader::FragmentLightingShader(GLuint shader_program)
    : Shader{shader_program, Gfx::SHADER_CAMERA_NORMAL_MATRIX | Gfx::SHADER_LIGHT_DATA_BLOCK}
    , material_block{Gfx::OGL::get_uniform_block_index(shader_program, "MaterialBlock")}
{
    Gfx::OGL::bind_uniform_block(shader_program, material_block, MATERIAL_BLOCK_BINDING);
}

FragmentLightingColorShader::FragmentLightingColorShader(GLuint shader_program)
    : FragmentLightingShader{shader_program}
    , color{Gfx::OGL::get_uniform_location(shader_program, "uColor")}
{
}

////////////////////////////////////////

FragmentLightingMaterial::FragmentLightingMaterial(const Gfx::Shader &shader)
    : Material{shader}
{
}

void FragmentLightingMaterial::apply_properties() const
{
}

FragmentLightingColorMaterial::FragmentLightingColorMaterial(const Gfx::Shader &shader)
    : FragmentLightingMaterial{shader}
{
}

void FragmentLightingColorMaterial::apply_properties() const
{
    FragmentLightingMaterial::apply_properties();
    const auto &shader = static_cast<const FragmentLightingColorShader &>(this->shader);
    Gfx::OGL::set_uniform(shader.color, color);
}
} // namespace Game
