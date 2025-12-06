#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "ECS.hpp"
#include "Shader.hpp"

namespace Age::Gfx
{
using MaterialId = std::uint32_t;

struct Material
{
    Shader &shader;

    Material(Shader &shader);

    virtual void apply_properties() const = 0;
};

struct MaterialRef
{
    static constexpr auto TYPE{Core::ComponentType::MATERIAL};

    MaterialId material_id{};
};

extern std::vector<std::unique_ptr<Material>> g_materials;

void init_material_system();

template <typename TMaterial>
TMaterial &create_material(MaterialId material_id, ShaderId shader_id)
{
    if (material_id >= g_materials.size())
        g_materials.resize(material_id + 1);

    g_materials[material_id] = std::make_unique<TMaterial>(get_shader(shader_id));
    return static_cast<TMaterial &>(*g_materials[material_id]);
}

Material &get_material(MaterialId material_id);

const Material &use_material(MaterialId material_id);
void release_used_material();
} // namespace Age::Gfx
