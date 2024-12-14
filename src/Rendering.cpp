#include "Rendering.hpp"

namespace Age::Gfx
{
// static std::uint16_t entity_draw_calls[];
// static DrawCall draw_calls[];
// static DrawCallKey draw_call_keys[];
// static std::size_t draw_call_key_count{};

// create: drawcall, index to drawcall, sortkey, rendercomponent
void add_entity_to_rendering(ECS::EntityId entity_id)
{
}

// remove: rendercomponent; zero: drawcall, index to drawcall
void remove_entity_from_rendering(ECS::EntityId entity_id)
{
}

void queue_draw_calls(const RenderComponent *, DrawCallKey *, std::size_t count)
{
}

void sort_draw_calls(DrawCallKey *, std::size_t count)
{
}

void execute_draw_calls(const DrawCallKey *, std::size_t count)
{
}
} // namespace Age::Gfx
