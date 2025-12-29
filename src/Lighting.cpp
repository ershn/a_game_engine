#include <span>

#include "Lighting.hpp"
#include "Transform.hpp"

namespace Age::Gfx
{
namespace
{
void calc_directional_light(Core::EntityId entity_id, const Math::Matrix4 &wv_matrix, LightBlock::Light &computed_light)
{
    auto [transform, directional_light] =
        Core::get_entity_components<const Core::Transform, const DirectionalLight>(entity_id);

    computed_light = {
        .view_position{wv_matrix * Math::Vector4{transform.position, 0.0f}},
        .intensity{directional_light.light_intensity}
    };
}

void calc_point_light(Core::EntityId entity_id, const Math::Matrix4 &wv_matrix, LightBlock::Light &computed_light)
{
    auto [transform, point_light] = Core::get_entity_components<const Core::Transform, const PointLight>(entity_id);

    computed_light = {
        .view_position{wv_matrix * Math::Vector4{transform.position, 1.0f}}, .intensity{point_light.light_intensity}
    };
}
} // namespace

void update_light_group_buffer(const Math::Matrix4 &wv_matrix, const LightGroup &light_group)
{
    LightBlock light_block{};

    const auto &light_settings = Core::get_entity_component<LightSettings>(light_group.light_settings_id);
    light_block.ambient_light_intensity = light_settings.ambient_light_intensity;
    light_block.light_attenuation = light_settings.light_attenuation;
    light_block.max_intensity = light_settings.max_intensity;

    for (std::size_t index{}; index < std::size(light_group.light_ids); ++index)
    {
        auto light_id = light_group.light_ids[index];
        if (light_id == 0)
            break;

        switch (light_group.light_types[index])
        {
        case Core::ComponentType::DIRECTIONAL_LIGHT:
            calc_directional_light(light_id, wv_matrix, light_block.lights[index]);
            break;
        case Core::ComponentType::POINT_LIGHT:
            calc_point_light(light_id, wv_matrix, light_block.lights[index]);
            break;
        }
    }

    light_group.uniform_buffer.update(light_block);
}
} // namespace Age::Gfx
