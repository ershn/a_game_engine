#pragma once

#include <concepts>
#include <cstdint>
#include <memory>
#include <vector>

#include "Components.hpp"
#include "IdGenerator.hpp"
#include "Shader.hpp"

namespace Age::Gfx
{
enum struct MaterialId : std::uint32_t
{
};

constexpr std::size_t to_index(MaterialId id)
{
    return static_cast<std::size_t>(id);
}

struct Material
{
    Shader &shader;
    const RenderPipelineState render_state{};
    const DrawQueue draw_queue{};

    Material(Shader &shader, RenderPipelineState render_state, DrawQueue draw_queue);

    virtual void apply_properties() const = 0;
};

template <std::derived_from<Material> TMaterial>
struct IdentifiedMaterial
{
    MaterialId id{};
    TMaterial &material;
};

struct MaterialRef
{
    static constexpr auto TYPE{Core::ComponentType::MATERIAL};

    MaterialId material_id{};
};

extern Util::IdGenerator<MaterialId> g_material_id_generator;
extern std::vector<std::unique_ptr<Material>> g_materials;

void init_material_system();

template <std::derived_from<Material> TMaterial>
IdentifiedMaterial<TMaterial> create_material_impl(
    Shader &shader, RenderPipelineStateMask render_state_mask, RenderPipelineState render_state, DrawQueue draw_queue
)
{
    MaterialId material_id{g_material_id_generator.generate()};
    std::size_t material_index{to_index(material_id)};

    if (material_index >= g_materials.size())
        g_materials.resize(material_index + 1);

    auto combined_render_state =
        RenderPipelineState::from(shader.render_state & ~render_state_mask | render_state & render_state_mask);
    g_materials[material_index] = std::make_unique<TMaterial>(shader, combined_render_state, draw_queue);
    return {material_id, static_cast<TMaterial &>(*g_materials[material_index])};
}

template <std::derived_from<Material> TMaterial>
IdentifiedMaterial<TMaterial> create_material(ShaderId shader_id)
{
    Shader &shader{get_shader(shader_id)};
    return create_material_impl<TMaterial>(shader, {}, {}, shader.draw_queue);
}

template <std::derived_from<Material> TMaterial>
IdentifiedMaterial<TMaterial> create_material(
    ShaderId shader_id, RenderPipelineStateMask render_state_mask, RenderPipelineState render_state
)
{
    Shader &shader{get_shader(shader_id)};
    return create_material_impl<TMaterial>(shader, render_state_mask, render_state, shader.draw_queue);
}

template <std::derived_from<Material> TMaterial>
IdentifiedMaterial<TMaterial> create_material(ShaderId shader_id, DrawQueue draw_queue)
{
    Shader &shader{get_shader(shader_id)};
    return create_material_impl<TMaterial>(shader, {}, {}, draw_queue);
}

template <std::derived_from<Material> TMaterial>
IdentifiedMaterial<TMaterial> create_material(
    ShaderId shader_id,
    RenderPipelineStateMask render_state_mask,
    RenderPipelineState render_state,
    DrawQueue draw_queue
)
{
    Shader &shader{get_shader(shader_id)};
    return create_material_impl<TMaterial>(shader, render_state_mask, render_state, draw_queue);
}

Material &get_material(MaterialId material_id);

const Material &use_material(MaterialId material_id);
void release_used_material();
} // namespace Age::Gfx
