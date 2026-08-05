#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <span>
#include <tuple>
#include <utility>
#include <vector>

#include "components.hpp"
#include "id_generator.hpp"
#include "inplace_list.hpp"
#include "memory.hpp"
#include "multi_span.hpp"
#include "utils.hpp"

namespace Age::Core
{
class ChunkArray
{
    static constexpr std::uint32_t DEFAULT_CAPACITY{8};
    static constexpr double CAPACITY_GROWTH_FACTOR{2.0};

    std::byte **_chunk_ptrs{};
    std::uint32_t _capacity{1};
    std::uint32_t _size{0};

  public:
    std::uint32_t size() const;

    std::byte *operator[](std::uint32_t index) const;

    std::byte *back() const;

    void push_back(std::byte *chunk);

  private:
    void grow_capacity();
};

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
    ChunkArray chunks{};
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

void init_ecs(std::size_t user_component_type_count);

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

    std::byte *chunk{};
    if (entity_index == 0)
    {
        chunk = static_cast<std::byte *>(g_chunk_allocator.get_chunk());
        archetype.chunks.push_back(chunk);
    }
    else
    {
        chunk = archetype.chunks.back();
    }

    EntityId *entity_id_array{reinterpret_cast<EntityId *>(chunk)};
    entity_id_array[entity_index] = entity_id;

    std::array<std::size_t, sizeof...(TComponents)> cmpt_types{static_cast<std::size_t>(TComponents::TYPE)...};
    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> cmpt_archetype_id_arrays{
        &g_component_archetype_ids[cmpt_types[IS]]...
    };
    std::array<std::size_t, sizeof...(TComponents)> cmpt_array_indices{static_cast<std::size_t>(
        std::find(cmpt_archetype_id_arrays[IS]->cbegin(), cmpt_archetype_id_arrays[IS]->cend(), archetype_id) -
        cmpt_archetype_id_arrays[IS]->cbegin()
    )...};
    std::array<ComponentOffset, sizeof...(TComponents)> cmpt_offsets{
        g_component_archetype_offsets[cmpt_types[IS]][cmpt_array_indices[IS]]...
    };
    std::array<std::byte *, sizeof...(TComponents)> cmpt_ptrs{
        chunk + cmpt_offsets[IS] + entity_index * sizeof(TComponents)...
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
    std::uint32_t chunk_index{entity_location.entity_index / archetype.entity_count_per_chunk};
    std::uint32_t chunk_entity_index{entity_location.entity_index % archetype.entity_count_per_chunk};
    std::byte *chunk_ptr{archetype.chunks[chunk_index]};

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

    return reinterpret_cast<TComponent *>(chunk_ptr + cmpt_offset)[chunk_entity_index];
}

template <typename... TComponents, std::size_t... IS>
std::tuple<TComponents &...> get_entity_components_impl(EntityId entity_id, std::index_sequence<IS...>)
{
    const EntityLocation &entity_location{g_entity_locations[entity_id - 1]};
    Archetype &archetype{g_archetypes[entity_location.archetype_id]};
    std::uint32_t chunk_index{entity_location.entity_index / archetype.entity_count_per_chunk};
    std::uint32_t chunk_entity_index{entity_location.entity_index % archetype.entity_count_per_chunk};
    std::byte *chunk_ptr{archetype.chunks[chunk_index]};

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

    return std::tie(reinterpret_cast<TComponents *>(chunk_ptr + cmpt_offsets[IS])[chunk_entity_index]...);
}

template <typename... TComponents>
std::tuple<TComponents &...> get_entity_components(EntityId entity_id)
{
    return get_entity_components_impl<TComponents...>(entity_id, std::index_sequence_for<TComponents...>{});
}

template <typename... TComponents, typename TFunctor, std::size_t... ISLess1, std::size_t... IS>
void execute_impl(TFunctor functor, std::index_sequence<ISLess1...>, std::index_sequence<IS...>)
{
    std::array<const std::vector<ArchetypeId> *, sizeof...(TComponents)> cmpt_archetype_id_vectors{
        &g_component_archetype_ids[static_cast<std::size_t>(TComponents::TYPE)]...
    };

    std::array<std::size_t, sizeof...(TComponents)> cmpt_archetype_indices{};
    std::array<std::size_t, sizeof...(TComponents)> cmpt_archetype_counts{cmpt_archetype_id_vectors[IS]->size()...};

    while (((cmpt_archetype_indices[IS] < cmpt_archetype_counts[IS]) && ...))
    {
        std::array<ArchetypeId, sizeof...(TComponents)> cmpt_archetype_ids{
            (*cmpt_archetype_id_vectors[IS])[cmpt_archetype_indices[IS]]...
        };

        if (((cmpt_archetype_ids[0] == cmpt_archetype_ids[ISLess1 + 1]) && ...))
        {
            const Archetype &archetype{g_archetypes[cmpt_archetype_ids[0]]};

            for (std::uint32_t chunk_index{}; chunk_index < archetype.chunks.size(); ++chunk_index)
            {
                std::byte *chunk_ptr{archetype.chunks[chunk_index]};
                std::array<ComponentOffset, sizeof...(TComponents)> cmpt_offsets{
                    g_component_archetype_offsets[static_cast<std::size_t>(TComponents::TYPE)]
                                                 [cmpt_archetype_indices[IS]]...
                };

                functor(
                    (archetype.entity_count - chunk_index * archetype.entity_count_per_chunk) %
                        archetype.entity_count_per_chunk,
                    reinterpret_cast<const EntityId *>(chunk_ptr),
                    reinterpret_cast<TComponents *>(chunk_ptr + cmpt_offsets[IS])...
                );
            }

            (++cmpt_archetype_indices[IS], ...);
        }
        else
        {
            for (std::size_t index{}; index < sizeof...(TComponents); ++index)
                cmpt_archetype_indices[index] += ((cmpt_archetype_ids[index] <= cmpt_archetype_ids[IS]) && ...);
        }
    }
}

template <typename... TComponents, typename TFunctor>
void execute_impl(TFunctor functor)
{
    execute_impl<TComponents...>(
        functor, std::make_index_sequence<sizeof...(TComponents) - 1>{}, std::index_sequence_for<TComponents...>{}
    );
}

template <typename... TComponents, typename TFunctor>
void execute_functor_impl(TFunctor functor, void (TFunctor::*)(std::uint32_t, TComponents *...) const)
{
    execute_impl<TComponents...>([functor](std::uint32_t entity_count, const EntityId *, TComponents *...components) {
        functor(entity_count, components...);
    });
}

template <typename... TComponents, typename TFunctor>
void execute_functor_impl(TFunctor functor, void (TFunctor::*)(std::uint32_t, TComponents *...))
{
    execute_impl<TComponents...>([functor](std::uint32_t entity_count, const EntityId *, TComponents *...components) {
        functor(entity_count, components...);
    });
}

template <typename... TComponents, typename TFunctor>
void execute_functor_impl(TFunctor functor, void (TFunctor::*)(std::uint32_t, const EntityId *, TComponents *...) const)
{
    execute_impl<TComponents...>(functor);
}

template <typename... TComponents, typename TFunctor>
void execute_functor_impl(TFunctor functor, void (TFunctor::*)(std::uint32_t, const EntityId *, TComponents *...))
{
    execute_impl<TComponents...>(functor);
}

template <typename... TComponents, typename TFunctor>
void execute_functor_impl(TFunctor functor, void (TFunctor::*)(const MultiSpan<TComponents...> &) const)
{
    execute_impl<TComponents...>([functor](std::uint32_t entity_count, const EntityId *, TComponents *...components) {
        functor(MultiSpan{entity_count, components...});
    });
}

template <typename... TComponents, typename TFunctor>
void execute_functor_impl(TFunctor functor, void (TFunctor::*)(const MultiSpan<TComponents...> &))
{
    execute_impl<TComponents...>([functor](std::uint32_t entity_count, const EntityId *, TComponents *...components) {
        functor(MultiSpan{entity_count, components...});
    });
}

template <typename... TComponents, typename TFunctor>
void execute_functor_impl(TFunctor functor, void (TFunctor::*)(const MultiSpan<const EntityId, TComponents...> &) const)
{
    execute_impl<TComponents...>(
        [functor](std::uint32_t entity_count, const EntityId *entity_ids, TComponents *...components) {
            functor(MultiSpan{entity_count, entity_ids, components...});
        }
    );
}

template <typename... TComponents, typename TFunctor>
void execute_functor_impl(TFunctor functor, void (TFunctor::*)(const MultiSpan<const EntityId, TComponents...> &))
{
    execute_impl<TComponents...>(
        [functor](std::uint32_t entity_count, const EntityId *entity_ids, TComponents *...components) {
            functor(MultiSpan{entity_count, entity_ids, components...});
        }
    );
}

template <typename TFunctor>
void execute(TFunctor functor)
{
    execute_functor_impl(functor, &TFunctor::operator());
}

template <typename... TComponents>
void execute(void (*function)(std::uint32_t, TComponents *...))
{
    execute_impl<TComponents...>([function](std::uint32_t entity_count, const EntityId *, TComponents *...components) {
        function(entity_count, components...);
    });
}

template <typename... TComponents>
void execute(void (*function)(std::uint32_t, const EntityId *, TComponents *...))
{
    execute_impl<TComponents...>(function);
}

template <typename... TComponents, typename TFunctor>
void process_components_functor_impl(TFunctor functor, void (TFunctor::*)(TComponents &...) const)
{
    execute_impl<TComponents...>(
        [functor](std::uint32_t entity_count, const EntityId *entity_ids, TComponents *...components) {
            for (std::uint32_t index{}; index < entity_count; ++index)
                functor(components[index]...);
        }
    );
}

template <typename... TComponents, typename TFunctor>
void process_components_functor_impl(TFunctor functor, void (TFunctor::*)(TComponents &...))
{
    execute_impl<TComponents...>(
        [functor](std::uint32_t entity_count, const EntityId *entity_ids, TComponents *...components) {
            for (std::uint32_t index{}; index < entity_count; ++index)
                functor(components[index]...);
        }
    );
}

template <typename... TComponents, typename TFunctor>
void process_components_functor_impl(TFunctor functor, void (TFunctor::*)(EntityId, TComponents &...) const)
{
    execute_impl<TComponents...>(
        [functor](std::uint32_t entity_count, const EntityId *entity_ids, TComponents *...components) {
            for (std::uint32_t index{}; index < entity_count; ++index)
                functor(entity_ids[index], components[index]...);
        }
    );
}

template <typename... TComponents, typename TFunctor>
void process_components_functor_impl(TFunctor functor, void (TFunctor::*)(EntityId, TComponents &...))
{
    execute_impl<TComponents...>(
        [functor](std::uint32_t entity_count, const EntityId *entity_ids, TComponents *...components) {
            for (std::uint32_t index{}; index < entity_count; ++index)
                functor(entity_ids[index], components[index]...);
        }
    );
}

template <typename TFunctor>
void process_components(TFunctor functor)
{
    process_components_functor_impl(functor, &TFunctor::operator());
}

template <typename... TComponents>
void process_components(void (*function)(TComponents &...))
{
    execute_impl<TComponents...>(
        [function](std::uint32_t entity_count, const EntityId *entity_ids, TComponents *...components) {
            for (std::uint32_t index{}; index < entity_count; ++index)
                function(components[index]...);
        }
    );
}

template <typename... TComponents>
void process_components(void (*function)(EntityId, TComponents &...))
{
    execute_impl<TComponents...>(
        [function](std::uint32_t entity_count, const EntityId *entity_ids, TComponents *...components) {
            for (std::uint32_t index{}; index < entity_count; ++index)
                function(entity_ids[index], components[index]...);
        }
    );
}

template <typename TContainer, typename... TComponents>
void fill_with_entity_components_impl(TContainer &cmpts_container, Util::TypePack<TComponents &...>)
{
    std::uint32_t entity_count{};
    execute([&](std::uint32_t count, TComponents *...components) {
        for (std::uint32_t index{}; index < count && entity_count < cmpts_container.max_size(); ++index, ++entity_count)
            cmpts_container.emplace_back(components[index]...);
    });
}

template <typename TContainer, typename... TComponents>
void fill_with_entity_components_impl(TContainer &cmpts_container, Util::TypePack<TComponents *...>)
{
    std::uint32_t entity_count{};
    execute([&](std::uint32_t count, TComponents *...components) {
        for (std::uint32_t index{}; index < count && entity_count < cmpts_container.max_size(); ++index, ++entity_count)
            cmpts_container.emplace_back(&components[index]...);
    });
}

template <typename TContainer, typename... TComponents>
void fill_with_entity_components_impl(TContainer &cmpts_container, Util::TypePack<EntityId, TComponents &...>)
{
    std::uint32_t entity_count{};
    execute([&](std::uint32_t count, const EntityId *entity_ids, TComponents *...components) {
        for (std::uint32_t index{}; index < count && entity_count < cmpts_container.max_size(); ++index, ++entity_count)
            cmpts_container.emplace_back(entity_ids[index], components[index]...);
    });
}

template <typename TContainer, typename... TComponents>
void fill_with_entity_components_impl(TContainer &cmpts_container, Util::TypePack<EntityId, TComponents *...>)
{
    std::uint32_t entity_count{};
    execute([&](std::uint32_t count, const EntityId *entity_ids, TComponents *...components) {
        for (std::uint32_t index{}; index < count && entity_count < cmpts_container.max_size(); ++index, ++entity_count)
            cmpts_container.emplace_back(entity_ids[index], &components[index]...);
    });
}

template <typename TContainer>
void fill_with_entity_components(TContainer &cmpts_container)
{
    fill_with_entity_components_impl(
        cmpts_container, typename Util::TypeArgs<typename TContainer::value_type>::TypePack{}
    );
}
} // namespace Age::Core
