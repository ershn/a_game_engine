#include "Transform.hpp"
#include "Transformations.hpp"

namespace Age::Core
{
std::ostream &operator<<(std::ostream &out, const Transform &transform)
{
    out << "{.position" << transform.position << ", .orientation" << transform.orientation << ", .scale"
        << transform.scale << "}";
    return out;
}

Math::Matrix4 transform_matrix(const Transform &transform)
{
    return Math::translation_matrix(transform.position) * Math::affine_rotation_matrix(transform.orientation) *
           Math::affine_scaling_matrix(transform.scale);
}
} // namespace Age::Core
