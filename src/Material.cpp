#include <limits>

#include "Material.hpp"

namespace Age::Gfx
{
namespace
{
constexpr MaterialId NULL_MATERIAL_ID{std::numeric_limits<MaterialId>::max()};

MaterialId s_used_material_id{NULL_MATERIAL_ID};
} // namespace

Material::Material(Shader &shader)
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

void release_used_material()
{
    s_used_material_id = NULL_MATERIAL_ID;
}
} // namespace Age::Gfx
