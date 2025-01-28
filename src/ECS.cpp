#include <cstring>
#include <unordered_map>

#include "ECS.hpp"
#include "Hash.hpp"

#include "game/Game.hpp"

template <>
struct std::hash<std::span<const Age::Core::ComponentType>>
{
    inline std::size_t operator()(std::span<const Age::Core::ComponentType> types) const noexcept
    {
        std::size_t seed{};
        for (auto type : types)
        {
            Age::Util::hash_combine(seed, std::hash<Age::Core::ComponentType>{}(type));
        }
        return seed;
    }
};

namespace Age::Core
{
Memory::PoolAllocator<1U << 18, ARCHETYPE_CHUNK_SIZE> g_chunk_allocator{};

std::vector<Archetype> g_archetypes{};
// Stored ArchetypeIds are +1
static std::unordered_map<std::size_t, ArchetypeId> s_component_types_to_archetype_ids{};

std::vector<std::vector<ArchetypeId>> g_component_archetype_ids{};
std::vector<std::vector<ComponentOffset>> g_component_archetype_offsets{};

Util::IdGenerator<EntityId> g_entity_id_generator{0};
std::vector<EntityLocation> g_entity_locations{};

void init_ecs()
{
    std::size_t component_type_count{static_cast<std::size_t>(ComponentType::LAST_VALUE) +
                                     Game::get_component_type_count()};

    g_archetypes.reserve(256);
    s_component_types_to_archetype_ids.reserve(256);

    g_component_archetype_ids.resize(component_type_count);
    g_component_archetype_offsets.resize(component_type_count);

    for (std::size_t index{}; index < component_type_count; ++index)
    {
        g_component_archetype_ids[index].reserve(8);
        g_component_archetype_offsets[index].reserve(8);
    }

    g_entity_locations.reserve(1ULL << 14);
}

ArchetypeRef get_or_create_archetype(std::span<const ComponentType> component_types,
                                     std::span<const std::size_t> component_sizes)
{
    std::size_t hash{std::hash<std::span<const ComponentType>>{}(component_types)};
    ArchetypeId &stored_archetype_id{s_component_types_to_archetype_ids[hash]};
    if (stored_archetype_id == 0)
    {
        ArchetypeId archetype_id{static_cast<ArchetypeId>(g_archetypes.size())};
        stored_archetype_id = archetype_id + 1;
        return {archetype_id,
                g_archetypes.emplace_back(create_archetype(archetype_id, component_types, component_sizes))};
    }
    else
    {
        ArchetypeId archetype_id{stored_archetype_id - 1U};
        return {archetype_id, g_archetypes[archetype_id]};
    }
}

Archetype create_archetype(ArchetypeId archetype_id,
                           std::span<const ComponentType> component_types,
                           std::span<const std::size_t> component_sizes)
{
    using Memory::padding_to;

    Archetype archetype{};

    std::size_t entity_size{sizeof(EntityId)};
    for (auto size : component_sizes)
    {
        entity_size += size;
    }

    archetype.entity_count_per_chunk = static_cast<std::uint16_t>(ARCHETYPE_CHUNK_SIZE / entity_size);

    if (component_sizes.size() > 0)
    {
        while (true)
        {
            std::size_t padding{padding_to<8>(sizeof(EntityId) * archetype.entity_count_per_chunk)};
            for (std::size_t index{}; index < component_sizes.size() - 1; ++index)
            {
                padding += padding_to<8>(component_sizes[index] * archetype.entity_count_per_chunk);
            }

            if (entity_size * archetype.entity_count_per_chunk + padding > ARCHETYPE_CHUNK_SIZE)
                --archetype.entity_count_per_chunk;
            else
                break;
        }
    }

    archetype.chunks.reserve(8);

    std::size_t in_chunk_offset{sizeof(EntityId) * archetype.entity_count_per_chunk};
    in_chunk_offset += padding_to<8>(in_chunk_offset);
    for (std::size_t index{}; index < component_types.size(); ++index)
    {
        std::size_t component_type{static_cast<std::size_t>(component_types[index])};

        g_component_archetype_ids[component_type].emplace_back(archetype_id);
        g_component_archetype_offsets[component_type].emplace_back(static_cast<ComponentOffset>(in_chunk_offset));

        std::size_t component_size{component_sizes[index]};
        in_chunk_offset += component_size * archetype.entity_count_per_chunk;
        in_chunk_offset += padding_to<8>(in_chunk_offset);
    }

    return archetype;
}
} // namespace Age::Core
