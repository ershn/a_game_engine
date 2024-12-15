#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <tuple>
#include <utility>
#include <vector>

#include "Components.hpp"
#include "Hash.hpp"
#include "IdGenerator.hpp"

namespace Age::ECS
{
constexpr std::uint16_t ARCHETYPE_CHUNK_SIZE{1U << 14};

using EntityId = std::uint64_t;
using ArchetypeId = std::uint16_t;
using ComponentOffset = std::uint16_t;

struct EntityLocation
{
    ArchetypeId archetype_id{};
    std::uint32_t entity_index{};
};

struct Archetype
{
    std::vector<void *> chunks{};
    std::uint32_t entity_count{};
    std::uint16_t entity_count_per_chunk{};
};

struct ArchetypeRef
{
    ArchetypeId archetype_id{};
    Archetype &archetype;
};

extern std::vector<Archetype> g_archetypes;

extern std::array<std::vector<ArchetypeId>, COMPONENT_TYPE_COUNT> g_component_archetype_ids;
extern std::array<std::vector<ComponentOffset>, COMPONENT_TYPE_COUNT> g_component_archetype_offsets;

extern std::vector<EntityLocation> g_entity_locations;

void init_ecs();

template <std::size_t N>
struct SortedComponentAttrs
{
    std::array<ComponentType, N> component_types;
    std::array<std::size_t, N> component_sizes;
};

template <typename... TComponents>
consteval SortedComponentAttrs<sizeof...(TComponents)> get_sorted_component_attrs()
{
    using AttrPair = std::pair<ComponentType, std::size_t>;

    std::array<AttrPair, sizeof...(TComponents)> attr_pairs{std::make_pair(TComponents::TYPE, sizeof(TComponents))...};
    std::sort(attr_pairs.begin(), attr_pairs.end(),
              [](const AttrPair &a, const AttrPair &b) { return a.first < b.first; });

    SortedComponentAttrs<sizeof...(TComponents)> sorted_attrs{};
    for (std::size_t index{}; index < sizeof...(TComponents); ++index)
    {
        sorted_attrs.component_types[index] = attr_pairs[index].first;
        sorted_attrs.component_sizes[index] = attr_pairs[index].second;
    }
    return sorted_attrs;
}

template <typename... TComponents>
EntityId create_entity(const TComponents &...components)
{
    constexpr static std::array<ComponentType, sizeof...(TComponents)> component_types{TComponents::TYPE...};
    constexpr static std::array<std::size_t, sizeof...(TComponents)> component_sizes{sizeof(TComponents)...};
    constexpr static auto sorted_component_attrs{get_sorted_component_attrs<TComponents...>()};
    std::array<const void *, sizeof...(TComponents)> component_ptrs{&components...};

    ArchetypeRef archetype_ref{
        get_or_create_archetype(sorted_component_attrs.component_types, sorted_component_attrs.component_sizes)};
    return add_entity_to_archetype(archetype_ref.archetype_id, archetype_ref.archetype, component_types, component_ptrs,
                                   component_sizes);
}

void remove_entity();

void add_component_to_entity();
void remove_component_from_entity();

ArchetypeRef get_or_create_archetype(std::span<const ComponentType> component_types,
                                     std::span<const std::size_t> component_sizes);

Archetype create_archetype(ArchetypeId archetype_id, std::span<const ComponentType> component_types,
                           std::span<const std::size_t> component_sizes);

EntityId add_entity_to_archetype(ArchetypeId archetype_id, Archetype &archetype,
                                 std::span<const ComponentType> component_types, std::span<const void *> component_ptrs,
                                 std::span<const std::size_t> component_sizes);

template <typename... TComponents, std::size_t... IS>
std::tuple<TComponents &...> get_entity_components_impl(EntityId entity_id, std::index_sequence<IS...>)
{
    const EntityLocation &entity_location{g_entity_locations[entity_id]};
    Archetype &archetype{g_archetypes[entity_location.archetype_id]};
    std::size_t chunk_index{entity_location.entity_index / archetype.entity_count_per_chunk};
    std::size_t chunk_entity_index{entity_location.entity_index % archetype.entity_count_per_chunk};
    char *chunk_ptr{static_cast<char *>(archetype.chunks[chunk_index])};

    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> cmpt_archetype_ids{
        &g_component_archetype_ids[static_cast<std::size_t>(TComponents::TYPE)]...};
    std::array<std::ptrdiff_t, sizeof...(TComponents)> cmpt_array_offsets{
        (std::find(cmpt_archetype_ids[IS]->cbegin(), cmpt_archetype_ids[IS]->cend(), entity_location.archetype_id) -
         cmpt_archetype_ids[IS]->cbegin())...};
    std::array<std::size_t, sizeof...(TComponents)> cmpt_offsets{
        g_component_archetype_offsets[static_cast<std::size_t>(TComponents::TYPE)][cmpt_array_offsets[IS]]...};

    return std::tie(
        static_cast<TComponents *>(static_cast<void *>(chunk_ptr + cmpt_offsets[IS]))[chunk_entity_index]...);
}

template <typename... TComponents>
std::tuple<TComponents &...> get_entity_components(EntityId entity_id)
{
    return get_entity_components_impl<TComponents...>(entity_id, std::index_sequence_for<TComponents...>{});
}

template <typename... TComponents, std::size_t... ISLess1, std::size_t... IS>
void process_components_impl(std::function<void(TComponents &...)> system_function, std::index_sequence<ISLess1...>,
                             std::index_sequence<IS...>)
{
    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> component_archetype_ids{
        &g_component_archetype_ids[static_cast<std::size_t>(TComponents::TYPE)]...};

    std::array<std::size_t, sizeof...(TComponents)> component_archetype_indexes{};
    std::array<std::size_t, sizeof...(TComponents)> component_archetype_counts{component_archetype_ids[IS]->size()...};

    while (((component_archetype_indexes[IS] < component_archetype_counts[IS]) && ...))
    {
        std::array<ArchetypeId, sizeof...(TComponents)> archetype_ids{
            (*component_archetype_ids[IS])[component_archetype_indexes[IS]]...};

        if (((archetype_ids[0] == archetype_ids[ISLess1 + 1]) && ...))
        {
            const Archetype &archetype{g_archetypes[archetype_ids[0]]};
            std::size_t entity_index{};

            for (decltype(archetype.chunks)::size_type chunk_index{}; chunk_index < archetype.chunks.size();
                 ++chunk_index)
            {
                char *chunk_ptr{static_cast<char *>(archetype.chunks[chunk_index])};
                std::array<ComponentOffset, sizeof...(TComponents)> component_offsets{
                    g_component_archetype_offsets[static_cast<std::size_t>(TComponents::TYPE)]
                                                 [component_archetype_indexes[IS]]...};

                for (std::size_t chunk_entity_index{};
                     chunk_entity_index < archetype.entity_count_per_chunk && entity_index < archetype.entity_count;
                     ++chunk_entity_index, ++entity_index)
                {
                    system_function(static_cast<TComponents *>(
                        static_cast<void *>(chunk_ptr + component_offsets[IS]))[chunk_entity_index]...);
                }
            }

            (component_archetype_indexes[IS]++, ...);
        }
        else
        {
            for (std::size_t index{}; index < sizeof...(TComponents); ++index)
            {
                component_archetype_indexes[index] += ((archetype_ids[index] <= archetype_ids[IS]) && ...);
            }
        }
    }
}

template <typename... TComponents>
void process_components(std::function<void(TComponents &...)> system_function)
{
    process_components_impl<TComponents...>(system_function, std::make_index_sequence<sizeof...(TComponents) - 1>{},
                                            std::index_sequence_for<TComponents...>{});
}

template <typename... TComponents, std::size_t... ISLess1, std::size_t... IS>
void process_components_impl(std::function<void(EntityId, TComponents &...)> system_function,
                             std::index_sequence<ISLess1...>, std::index_sequence<IS...>)
{
    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> component_archetype_ids{
        &g_component_archetype_ids[static_cast<std::size_t>(TComponents::TYPE)]...};

    std::array<std::size_t, sizeof...(TComponents)> component_archetype_indexes{};
    std::array<std::size_t, sizeof...(TComponents)> component_archetype_counts{component_archetype_ids[IS]->size()...};

    while (((component_archetype_indexes[IS] < component_archetype_counts[IS]) && ...))
    {
        std::array<ArchetypeId, sizeof...(TComponents)> archetype_ids{
            (*component_archetype_ids[IS])[component_archetype_indexes[IS]]...};

        if (((archetype_ids[0] == archetype_ids[ISLess1 + 1]) && ...))
        {
            const Archetype &archetype{g_archetypes[archetype_ids[0]]};
            std::size_t entity_index{};

            for (decltype(archetype.chunks)::size_type chunk_index{}; chunk_index < archetype.chunks.size();
                 ++chunk_index)
            {
                char *chunk_ptr{static_cast<char *>(archetype.chunks[chunk_index])};
                std::array<ComponentOffset, sizeof...(TComponents)> component_offsets{
                    g_component_archetype_offsets[static_cast<std::size_t>(TComponents::TYPE)]
                                                 [component_archetype_indexes[IS]]...};

                for (std::size_t chunk_entity_index{};
                     chunk_entity_index < archetype.entity_count_per_chunk && entity_index < archetype.entity_count;
                     ++chunk_entity_index, ++entity_index)
                {
                    system_function(
                        static_cast<const EntityId *>(static_cast<const void *>(chunk_ptr))[chunk_entity_index],
                        static_cast<TComponents *>(
                            static_cast<void *>(chunk_ptr + component_offsets[IS]))[chunk_entity_index]...);
                }
            }

            (component_archetype_indexes[IS]++, ...);
        }
        else
        {
            for (std::size_t index{}; index < sizeof...(TComponents); ++index)
            {
                component_archetype_indexes[index] += ((archetype_ids[index] <= archetype_ids[IS]) && ...);
            }
        }
    }
}

template <typename... TComponents>
void process_components(std::function<void(EntityId, TComponents &...)> system_function)
{
    process_components_impl<TComponents...>(system_function, std::make_index_sequence<sizeof...(TComponents) - 1>{},
                                            std::index_sequence_for<TComponents...>{});
}
} // namespace Age::ECS
