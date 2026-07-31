#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "ECS.hpp"
#include "Shader.hpp"

namespace Age::Gfx
{
using MaterialId = std::uint16_t;

struct Material
{
    Shader &shader;
    const RenderPipelineState render_state{};
    const DrawQueue draw_queue{};

    Material(Shader &shader, RenderPipelineState render_state, DrawQueue draw_queue);

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
TMaterial &create_material_impl(
    MaterialId material_id,
    Shader &shader,
    RenderPipelineStateMask render_state_mask,
    RenderPipelineState render_state,
    DrawQueue draw_queue
)
{
    if (material_id >= g_materials.size())
        g_materials.resize(material_id + 1);

    auto combined_render_state =
        RenderPipelineState::from(shader.render_state & ~render_state_mask | render_state & render_state_mask);
    g_materials[material_id] = std::make_unique<TMaterial>(shader, combined_render_state, draw_queue);
    return static_cast<TMaterial &>(*g_materials[material_id]);
}

template <typename TMaterial>
TMaterial &create_material(MaterialId material_id, ShaderId shader_id)
{
    Shader &shader{get_shader(shader_id)};
    return create_material_impl<TMaterial>(material_id, shader, {}, {}, shader.draw_queue);
}

template <typename TMaterial>
TMaterial &create_material(
    MaterialId material_id,
    ShaderId shader_id,
    RenderPipelineStateMask render_state_mask,
    RenderPipelineState render_state
)
{
    Shader &shader{get_shader(shader_id)};
    return create_material_impl<TMaterial>(material_id, shader, render_state_mask, render_state, shader.draw_queue);
}

template <typename TMaterial>
TMaterial &create_material(MaterialId material_id, ShaderId shader_id, DrawQueue draw_queue)
{
    Shader &shader{get_shader(shader_id)};
    return create_material_impl<TMaterial>(material_id, shader, {}, {}, draw_queue);
}

template <typename TMaterial>
TMaterial &create_material(
    MaterialId material_id,
    ShaderId shader_id,
    RenderPipelineStateMask render_state_mask,
    RenderPipelineState render_state,
    DrawQueue draw_queue
)
{
    Shader &shader{get_shader(shader_id)};
    return create_material_impl<TMaterial>(material_id, shader, render_state_mask, render_state, draw_queue);
}

Material &get_material(MaterialId material_id);

const Material &use_material(MaterialId material_id);
void release_used_material();
} // namespace Age::Gfx
