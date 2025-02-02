#include <cmath>

#include "Transformations.hpp"

namespace Age::Math
{
namespace
{
template <typename TMatrix>
TMatrix scaling_matrix(const Vector3 &scaling)
{
    TMatrix matrix{1.0f};
    matrix[0].x = scaling.x;
    matrix[1].y = scaling.y;
    matrix[2].z = scaling.z;
    return matrix;
}

template <typename TMatrix>
TMatrix rotation_matrix(const Quaternion &quat)
{
    TMatrix matrix{1.0f};
    matrix[0].x = 1.0f - 2.0f * quat.y * quat.y - 2.0f * quat.z * quat.z;
    matrix[0].y = 2.0f * quat.x * quat.y + 2.0f * quat.w * quat.z;
    matrix[0].z = 2.0f * quat.x * quat.z - 2.0f * quat.w * quat.y;
    matrix[1].x = 2.0f * quat.x * quat.y - 2.0f * quat.w * quat.z;
    matrix[1].y = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.z * quat.z;
    matrix[1].z = 2.0f * quat.y * quat.z + 2.0f * quat.w * quat.x;
    matrix[2].x = 2.0f * quat.x * quat.z + 2.0f * quat.w * quat.y;
    matrix[2].y = 2.0f * quat.y * quat.z - 2.0f * quat.w * quat.x;
    matrix[2].z = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.y * quat.y;
    return matrix;
}
} // namespace

Quaternion axis_angle_quaternion(const Vector3 &axis, float angle)
{
    Vector3 unit_axis{normalize(axis)};
    float half_angle{angle / 2.0f};
    float sin_half_angle{std::sin(half_angle)};

    return Quaternion{std::cos(half_angle), sin_half_angle * unit_axis.x, sin_half_angle * unit_axis.y,
                      sin_half_angle * unit_axis.z};
}

Matrix3 scaling_matrix(const Vector3 &scaling)
{
    return scaling_matrix<Matrix3>(scaling);
}

Matrix4 affine_scaling_matrix(const Vector3 &scaling)
{
    return scaling_matrix<Matrix4>(scaling);
}

Matrix3 rotation_matrix(const Quaternion &quat)
{
    return rotation_matrix<Matrix3>(quat);
}

Matrix4 affine_rotation_matrix(const Quaternion &quat)
{
    return rotation_matrix<Matrix4>(quat);
}

// y to z positive angle
//
// ^z
// |
// +-->y
Matrix3 x_rotation_matrix(float angle)
{
    Matrix3 matrix{1.0f};
    float cosine = std::cos(angle);
    float sinus = std::sin(angle);
    matrix[1].y = cosine;
    matrix[1].z = sinus;
    matrix[2].y = -sinus;
    matrix[2].z = cosine;
    return matrix;
}

// z to x positive angle
//
// ^x
// |
// +-->z
Matrix3 y_rotation_matrix(float angle)
{
    Matrix3 matrix{1.0f};
    float cosine = std::cos(angle);
    float sinus = std::sin(angle);
    matrix[0].x = cosine;
    matrix[0].z = -sinus;
    matrix[2].x = sinus;
    matrix[2].z = cosine;
    return matrix;
}

// x to y positive angle
//
// ^y
// |
// +-->x
Matrix3 z_rotation_matrix(float angle)
{
    Matrix3 matrix{1.0f};
    float cosine = std::cos(angle);
    float sinus = std::sin(angle);
    matrix[0].x = cosine;
    matrix[0].y = sinus;
    matrix[1].x = -sinus;
    matrix[1].y = cosine;
    return matrix;
}

Matrix4 translation_matrix(const Vector3 &pos)
{
    Matrix4 matrix{1.0f};
    matrix[3] = Vector4{pos, 1.0f};
    return matrix;
}

static Vector2 calc_zoom_values(float aspect_ratio, float vertical_fov)
{
    Vector2 zoom;
    zoom.y = 1.0f / tan(vertical_fov / 2.0f);
    zoom.x = zoom.y * (1.0f / aspect_ratio);
    return zoom;
}

Matrix4 perspective_matrix(float z_near, float z_far, float aspect_ratio, float vertical_fov)
{
    Vector2 zoom{calc_zoom_values(aspect_ratio, vertical_fov)};

    Matrix4 matrix{};
    matrix[0].x = zoom.x;
    matrix[1].y = zoom.y;
    matrix[2].z = (z_far + z_near) / (z_near - z_far);
    matrix[3].z = 2.0f * z_near * z_far / (z_near - z_far);
    matrix[2].w = -1.0f;
    return matrix;
}

void update_fov(Matrix4 &perspective_matrix, float aspect_ratio, float vertical_fov)
{
    Vector2 zoom{calc_zoom_values(aspect_ratio, vertical_fov)};

    perspective_matrix[0].x = zoom.x;
    perspective_matrix[1].y = zoom.y;
}

Matrix4 view_matrix(const Vector3 &camera_pos, const Vector3 &target_pos, const Vector3 &world_up)
{
    Matrix4 matrix{1.0f};

    Vector3 forward{normalize(camera_pos - target_pos)};
    Vector3 right{normalize(cross(world_up, forward))};
    Vector3 up{cross(forward, right)};

    matrix[0].x = right.x;
    matrix[0].y = right.y;
    matrix[0].z = right.z;
    matrix[1].x = up.x;
    matrix[1].y = up.y;
    matrix[1].z = up.z;
    matrix[2].x = forward.x;
    matrix[2].y = forward.y;
    matrix[2].z = forward.z;

    matrix.transpose();

    Matrix4 translation_matrix{1.0f};
    translation_matrix[3].x = -camera_pos.x;
    translation_matrix[3].y = -camera_pos.y;
    translation_matrix[3].z = -camera_pos.z;

    matrix *= translation_matrix;
    return matrix;
}
} // namespace Age::Math
