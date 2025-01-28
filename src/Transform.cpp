#include "Transform.hpp"

namespace Age::Core
{
std::ostream &operator<<(std::ostream &out, const Transform &transform)
{
    out << "{.position" << transform.position << ", .orientation" << transform.orientation << ", .scale"
        << transform.scale << "}";
    return out;
}
} // namespace Age::Core
