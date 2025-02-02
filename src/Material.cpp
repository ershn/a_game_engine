#include <limits>

#include "Material.hpp"

namespace Age::Gfx
{
Material::Material(const Shader &shader)
    : shader{shader}
{
}

std::vector<std::unique_ptr<Material>> g_materials{};

static GLuint s_used_shader_program{};
static MaterialId s_used_material_id{std::numeric_limits<MaterialId>::max()};

void init_material_system()
{
    g_materials.reserve(256);
}

Material &get_material(MaterialId material_id)
{
    return *g_materials[material_id];
}

const Material &use_material(MaterialId material_id)
{
    const Material &material{*g_materials[material_id]};

    if (material_id != s_used_material_id)
    {
        if (material.shader.shader_program != s_used_shader_program)
        {
            s_used_shader_program = material.shader.shader_program;
            OGL::use_program(s_used_shader_program);
        }

        s_used_material_id = material_id;
        material.apply_properties();
    }

    return material;
}
} // namespace Age::Gfx
