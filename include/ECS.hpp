#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <span>
#include <tuple>
#include <utility>
#include <vector>

#include "Components.hpp"
#include "Definitions.hpp"
#include "IdGenerator.hpp"
#include "Memory.hpp"

namespace Age::Core
{
inline constexpr std::uint16_t ARCHETYPE_CHUNK_SIZE{1U << 14};

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

extern Memory::PoolAllocator<1U << 18, ARCHETYPE_CHUNK_SIZE> g_chunk_allocator;

extern std::vector<Archetype> g_archetypes;

extern std::vector<std::vector<ArchetypeId>> g_component_archetype_ids;
extern std::vector<std::vector<ComponentOffset>> g_component_archetype_offsets;

extern Util::IdGenerator<EntityId> g_entity_id_generator;
extern std::vector<EntityLocation> g_entity_locations;

void init_ecs(const App::Definitions &definitions);

ArchetypeRef get_or_create_archetype(
    std::span<const ComponentType> component_types, std::span<const std::size_t> component_sizes
);

Archetype create_archetype(
    ArchetypeId archetype_id,
    std::span<const ComponentType> component_types,
    std::span<const std::size_t> component_sizes
);

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
    std::sort(attr_pairs.begin(), attr_pairs.end(), [](const AttrPair &a, const AttrPair &b) {
        return a.first < b.first;
    });

    SortedComponentAttrs<sizeof...(TComponents)> sorted_attrs{};
    for (std::size_t index{}; index < sizeof...(TComponents); ++index)
    {
        sorted_attrs.component_types[index] = attr_pairs[index].first;
        sorted_attrs.component_sizes[index] = attr_pairs[index].second;
    }
    return sorted_attrs;
}

template <typename... TComponents, std::size_t... IS>
EntityId add_entity_to_archetype(
    ArchetypeId archetype_id, Archetype &archetype, const TComponents &...components, std::index_sequence<IS...>
)
{
    EntityId entity_id{g_entity_id_generator.generate()};
    std::uint32_t entity_index{archetype.entity_count % archetype.entity_count_per_chunk};

    void *chunk{};
    if (entity_index == 0)
        chunk = archetype.chunks.emplace_back(g_chunk_allocator.get_chunk());
    else
        chunk = archetype.chunks.back();

    EntityId *entity_id_array{static_cast<EntityId *>(chunk)};
    entity_id_array[entity_index] = entity_id;

    std::array<std::size_t, sizeof...(TComponents)> cmpt_types{static_cast<std::size_t>(TComponents::TYPE)...};
    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> cmpt_archetype_id_arrays{
        &g_component_archetype_ids[cmpt_types[IS]]...
    };
    std::array<std::size_t, sizeof...(TComponents)> cmpt_array_indexes{static_cast<std::size_t>(
        std::find(cmpt_archetype_id_arrays[IS]->cbegin(), cmpt_archetype_id_arrays[IS]->cend(), archetype_id) -
        cmpt_archetype_id_arrays[IS]->cbegin()
    )...};
    std::array<ComponentOffset, sizeof...(TComponents)> cmpt_offsets{
        g_component_archetype_offsets[cmpt_types[IS]][cmpt_array_indexes[IS]]...
    };
    std::array<void *, sizeof...(TComponents)> cmpt_ptrs{
        static_cast<char *>(chunk) + cmpt_offsets[IS] + entity_index * sizeof(TComponents)...
    };
    (new (cmpt_ptrs[IS]) TComponents{components}, ...);

    ++archetype.entity_count;

    if (entity_id - 1 == g_entity_locations.size())
        g_entity_locations.emplace_back(archetype_id, entity_index);
    else
        g_entity_locations[entity_id - 1] = {archetype_id, entity_index};

    return entity_id;
}

template <typename... TComponents>
EntityId create_entity(const TComponents &...components)
{
    constexpr static auto sorted_attrs = get_sorted_component_attrs<TComponents...>();
    ArchetypeRef archetype_ref{get_or_create_archetype(sorted_attrs.component_types, sorted_attrs.component_sizes)};
    return add_entity_to_archetype<TComponents...>(
        archetype_ref.archetype_id, archetype_ref.archetype, components..., std::index_sequence_for<TComponents...>{}
    );
}

template <typename TComponent>
TComponent &get_entity_component(EntityId entity_id)
{
    const EntityLocation &entity_location{g_entity_locations[entity_id - 1]};
    Archetype &archetype{g_archetypes[entity_location.archetype_id]};
    std::size_t chunk_index{entity_location.entity_index / archetype.entity_count_per_chunk};
    std::size_t chunk_entity_index{entity_location.entity_index % archetype.entity_count_per_chunk};
    char *chunk_ptr{static_cast<char *>(archetype.chunks[chunk_index])};

    const std::vector<ArchetypeId> &cmpt_archetype_ids{
        g_component_archetype_ids[static_cast<std::size_t>(TComponent::TYPE)]
    };
    std::ptrdiff_t cmpt_array_offset{
        std::find(cmpt_archetype_ids.cbegin(), cmpt_archetype_ids.cend(), entity_location.archetype_id) -
        cmpt_archetype_ids.cbegin()
    };
    std::size_t cmpt_offset{
        g_component_archetype_offsets[static_cast<std::size_t>(TComponent::TYPE)][cmpt_array_offset]
    };

    return static_cast<TComponent *>(static_cast<void *>(chunk_ptr + cmpt_offset))[chunk_entity_index];
}

template <typename... TComponents, std::size_t... IS>
std::tuple<TComponents &...> get_entity_components_impl(EntityId entity_id, std::index_sequence<IS...>)
{
    const EntityLocation &entity_location{g_entity_locations[entity_id - 1]};
    Archetype &archetype{g_archetypes[entity_location.archetype_id]};
    std::size_t chunk_index{entity_location.entity_index / archetype.entity_count_per_chunk};
    std::size_t chunk_entity_index{entity_location.entity_index % archetype.entity_count_per_chunk};
    char *chunk_ptr{static_cast<char *>(archetype.chunks[chunk_index])};

    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> cmpt_archetype_ids{
        &g_component_archetype_ids[static_cast<std::size_t>(TComponents::TYPE)]...
    };
    std::array<std::ptrdiff_t, sizeof...(TComponents)> cmpt_array_offsets{
        (std::find(cmpt_archetype_ids[IS]->cbegin(), cmpt_archetype_ids[IS]->cend(), entity_location.archetype_id) -
         cmpt_archetype_ids[IS]->cbegin())...
    };
    std::array<std::size_t, sizeof...(TComponents)> cmpt_offsets{
        g_component_archetype_offsets[static_cast<std::size_t>(TComponents::TYPE)][cmpt_array_offsets[IS]]...
    };

    return std::tie(
        static_cast<TComponents *>(static_cast<void *>(chunk_ptr + cmpt_offsets[IS]))[chunk_entity_index]...
    );
}

template <typename... TComponents>
std::tuple<TComponents &...> get_entity_components(EntityId entity_id)
{
    return get_entity_components_impl<TComponents...>(entity_id, std::index_sequence_for<TComponents...>{});
}

template <typename... TComponents, std::size_t... ISLess1, std::size_t... IS>
void process_components_impl(
    std::function<void(TComponents &...)> system_function, std::index_sequence<ISLess1...>, std::index_sequence<IS...>
)
{
    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> component_archetype_ids{
        &g_component_archetype_ids[static_cast<std::size_t>(TComponents::TYPE)]...
    };

    std::array<std::size_t, sizeof...(TComponents)> component_archetype_indexes{};
    std::array<std::size_t, sizeof...(TComponents)> component_archetype_counts{component_archetype_ids[IS]->size()...};

    while (((component_archetype_indexes[IS] < component_archetype_counts[IS]) && ...))
    {
        std::array<ArchetypeId, sizeof...(TComponents)> archetype_ids{
            (*component_archetype_ids[IS])[component_archetype_indexes[IS]]...
        };

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
                                                 [component_archetype_indexes[IS]]...
                };

                for (std::size_t chunk_entity_index{};
                     chunk_entity_index < archetype.entity_count_per_chunk && entity_index < archetype.entity_count;
                     ++chunk_entity_index, ++entity_index)
                {
                    system_function(
                        static_cast<TComponents *>(
                            static_cast<void *>(chunk_ptr + component_offsets[IS])
                        )[chunk_entity_index]...
                    );
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
    process_components_impl<TComponents...>(
        system_function,
        std::make_index_sequence<sizeof...(TComponents) - 1>{},
        std::index_sequence_for<TComponents...>{}
    );
}

template <typename... TComponents>
void process_components(void (*system_function)(TComponents &...))
{
    process_components_impl<TComponents...>(
        std::function{system_function},
        std::make_index_sequence<sizeof...(TComponents) - 1>{},
        std::index_sequence_for<TComponents...>{}
    );
}

template <typename... TComponents, std::size_t... ISLess1, std::size_t... IS>
void process_components_impl(
    std::function<void(EntityId, TComponents &...)> system_function,
    std::index_sequence<ISLess1...>,
    std::index_sequence<IS...>
)
{
    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> component_archetype_ids{
        &g_component_archetype_ids[static_cast<std::size_t>(TComponents::TYPE)]...
    };

    std::array<std::size_t, sizeof...(TComponents)> component_archetype_indexes{};
    std::array<std::size_t, sizeof...(TComponents)> component_archetype_counts{component_archetype_ids[IS]->size()...};

    while (((component_archetype_indexes[IS] < component_archetype_counts[IS]) && ...))
    {
        std::array<ArchetypeId, sizeof...(TComponents)> archetype_ids{
            (*component_archetype_ids[IS])[component_archetype_indexes[IS]]...
        };

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
                                                 [component_archetype_indexes[IS]]...
                };

                for (std::size_t chunk_entity_index{};
                     chunk_entity_index < archetype.entity_count_per_chunk && entity_index < archetype.entity_count;
                     ++chunk_entity_index, ++entity_index)
                {
                    system_function(
                        static_cast<const EntityId *>(static_cast<const void *>(chunk_ptr))[chunk_entity_index],
                        static_cast<TComponents *>(
                            static_cast<void *>(chunk_ptr + component_offsets[IS])
                        )[chunk_entity_index]...
                    );
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
    process_components_impl<TComponents...>(
        system_function,
        std::make_index_sequence<sizeof...(TComponents) - 1>{},
        std::index_sequence_for<TComponents...>{}
    );
}

template <typename... TComponents>
void process_components(void (*system_function)(EntityId, TComponents &...))
{
    process_components_impl<TComponents...>(
        std::function{system_function},
        std::make_index_sequence<sizeof...(TComponents) - 1>{},
        std::index_sequence_for<TComponents...>{}
    );
}
} // namespace Age::Core
