#include <cstring>
#include <unordered_map>

#include "ECS.hpp"
#include "Memory.hpp"

template <>
struct std::hash<std::span<const Age::ECS::ComponentType>>
{
    inline std::size_t operator()(std::span<const Age::ECS::ComponentType> types) const noexcept
    {
        std::size_t seed{};
        for (auto type : types)
        {
            Age::Util::hash_combine(seed, std::hash<Age::ECS::ComponentType>{}(type));
        }
        return seed;
    }
};

namespace Age::ECS
{
static Memory::ChunkAllocator<1U << 18, ARCHETYPE_CHUNK_SIZE> s_chunk_allocator{};

std::vector<Archetype> g_archetypes{};
// Stored ArchetypeIds are +1
static std::unordered_map<std::size_t, ArchetypeId> s_component_types_to_archetype_ids{};

std::array<std::vector<ArchetypeId>, COMPONENT_TYPE_COUNT> g_component_archetype_ids{};
std::array<std::vector<ComponentOffset>, COMPONENT_TYPE_COUNT> g_component_archetype_offsets{};

static Util::IdGenerator<EntityId> s_entity_id_generator{0};
static std::vector<EntityLocation> s_entity_locations{};

void init_ecs()
{
    g_archetypes.reserve(256);
    s_component_types_to_archetype_ids.reserve(256);

    for (std::size_t index{}; index < COMPONENT_TYPE_COUNT; ++index)
    {
        g_component_archetype_ids[index].reserve(8);
        g_component_archetype_offsets[index].reserve(8);
    }

    s_entity_locations.reserve(1ULL << 14);
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

Archetype create_archetype(ArchetypeId archetype_id, std::span<const ComponentType> component_types,
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

EntityId add_entity_to_archetype(ArchetypeId archetype_id, Archetype &archetype,
                                 std::span<const ComponentType> component_types, std::span<const void *> component_ptrs,
                                 std::span<const std::size_t> component_sizes)
{
    EntityId entity_id{s_entity_id_generator.generate()};
    std::uint32_t entity_index{archetype.entity_count % archetype.entity_count_per_chunk};

    void *chunk{};
    if (entity_index == 0)
        chunk = archetype.chunks.emplace_back(s_chunk_allocator.get_chunk());
    else
        chunk = archetype.chunks.back();

    EntityId *entity_id_array{static_cast<EntityId *>(chunk)};
    entity_id_array[entity_index] = entity_id;

    for (std::size_t index{}; index < component_types.size(); ++index)
    {
        std::size_t cmpt_type{static_cast<std::size_t>(component_types[index])};
        const auto &cmpt_archetype_ids{g_component_archetype_ids[cmpt_type]};
        auto cmpt_arrays_offset{std::find(cmpt_archetype_ids.cbegin(), cmpt_archetype_ids.cend(), archetype_id) -
                                cmpt_archetype_ids.cbegin()};
        ComponentOffset cmpt_offset{g_component_archetype_offsets[cmpt_type][cmpt_arrays_offset]};
        void *cmpt_storage{static_cast<char *>(chunk) + cmpt_offset + entity_index * component_sizes[index]};

        std::memcpy(cmpt_storage, component_ptrs[index], component_sizes[index]);
    }

    ++archetype.entity_count;

    if (entity_id == s_entity_locations.size())
        s_entity_locations.emplace_back(archetype_id, entity_index);
    else
        s_entity_locations[entity_id] = {archetype_id, entity_index};

    return entity_id;
}
} // namespace Age::ECS
