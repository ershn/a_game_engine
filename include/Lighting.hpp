#pragma once

#include "Components.hpp"
#include "ECS.hpp"
#include "UniformBuffer.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
struct LightSettings
{
    static constexpr auto TYPE{Core::ComponentType::LIGHT_SETTINGS};

    Math::Vector4 ambient_light_intensity{};
    float light_attenuation{};
    float max_intensity{};
};

struct DirectionalLight
{
    static constexpr auto TYPE{Core::ComponentType::DIRECTIONAL_LIGHT};

    Math::Vector4 light_intensity{};
};

struct PointLight
{
    static constexpr auto TYPE{Core::ComponentType::POINT_LIGHT};

    Math::Vector4 light_intensity{};
};

struct LightBlock
{
    static constexpr std::size_t LIGHT_COUNT{4};

    struct Light
    {
        Math::Vector4 view_position{};
        Math::Vector4 intensity{};
    };

    Math::Vector4 ambient_light_intensity{};
    float light_attenuation{};
    float max_intensity{};
    float _padding_[2];
    Light lights[LIGHT_COUNT]{};
};

struct LightGroup
{
    static constexpr auto TYPE{Core::ComponentType::LIGHT_GROUP};

    Core::EntityId light_settings_id{};
    Core::EntityId light_ids[4]{};
    Core::ComponentType light_types[4]{};
    UniformBuffer<LightBlock> uniform_buffer;
    UniformBufferRangeId uniform_buffer_range_id{};
};

void update_light_group_buffer(const Math::Matrix4 &wv_matrix, const LightGroup &light_group);
} // namespace Age::Gfx
