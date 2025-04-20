#include "Logging.hpp"

namespace Age::Core
{
const std::chrono::time_zone *g_current_zone{};

void init_logging()
{
    try
    {
        g_current_zone = std::chrono::current_zone();
    }
    catch (const std::runtime_error &)
    {
    }
}
} // namespace Age::Core
