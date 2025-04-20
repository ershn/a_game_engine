#include "game/Game.hpp"
#include "game/Components.hpp"

namespace Game
{
using namespace Age;

extern const App::Definitions g_definitions{
    .component_type_count{ComponentType::LAST_VALUE - static_cast<std::size_t>(Core::ComponentType::LAST_VALUE)}
};
} // namespace Game
