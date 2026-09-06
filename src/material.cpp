#include <limits>
#include <type_traits>

#include "material.hpp"

namespace Age::Gfx
{
namespace
{
constexpr MaterialId NULL_MATERIAL_ID{std::numeric_limits<std::underlying_type_t<MaterialId>>::max()};

MaterialId s_used_material_id{NULL_MATERIAL_ID};
} // namespace

Material::Material(Shader &shader, RenderPipelineState render_state, DrawQueue draw_queue)
    : shader{shader}
    , render_state{render_state}
    , draw_queue{draw_queue}
{
}

Util::IdGenerator<MaterialId> g_material_id_generator{MaterialId{0}};
std::vector<std::unique_ptr<Material>> g_materials{};

void init_material_system()
{
    g_materials.reserve(256);
}

Material &get_material(MaterialId material_id)
{
    return *g_materials[to_index(material_id)];
}

const Material &use_material(MaterialId material_id)
{
    const Material &material{get_material(material_id)};

    if (material_id != s_used_material_id)
    {
        update_render_pipeline_state(material.render_state);
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
