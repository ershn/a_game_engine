#include <limits>

#include "Material.hpp"

namespace Age::Gfx
{
namespace
{
MaterialId s_used_material_id{std::numeric_limits<MaterialId>::max()};
}

Material::Material(const Shader &shader)
    : shader{shader}
{
}

std::vector<std::unique_ptr<Material>> g_materials{};

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
        use_shader(material.shader);
        material.apply_properties();
        s_used_material_id = material_id;
    }

    return material;
}
} // namespace Age::Gfx
