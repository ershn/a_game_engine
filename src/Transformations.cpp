#include <cmath>

#include "Transformations.hpp"

namespace Age::Math
{
Quaternion axis_angle_quaternion(const Vector3 &axis, float angle)
{
    Vector3 unit_axis{normalize(axis)};
    float half_angle{angle / 2.0f};
    float sin_half_angle{std::sin(half_angle)};

    return Quaternion{std::cos(half_angle), sin_half_angle * unit_axis.x,
                      sin_half_angle * unit_axis.y, sin_half_angle * unit_axis.z};
}

Matrix3 scaling_matrix(Vector3 scaling)
{
    Matrix3 matrix{1.0f};
    matrix[0].x = scaling.x;
    matrix[1].y = scaling.y;
    matrix[2].z = scaling.z;
    return matrix;
}

Matrix3 rotation_matrix(const Quaternion &quat)
{
    Matrix3 matrix{};
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

Matrix4 translation_matrix(Vector3 pos)
{
    Matrix4 matrix{1.0f};
    matrix[3] = Vector4{pos, 1.0f};
    return matrix;
}

void calc_zoom_values(float aspect_ratio, float vertical_fov, float &zoom_x, float &zoom_y)
{
    zoom_y = 1.0f / tan(vertical_fov / 2.0f);
    zoom_x = zoom_y * (1.0f / aspect_ratio);
}

Matrix4 perspective_matrix(float z_near, float z_far, float aspect_ratio, float vertical_fov)
{
    float zoom_x;
    float zoom_y;
    calc_zoom_values(aspect_ratio, vertical_fov, zoom_x, zoom_y);

    Matrix4 matrix{};
    matrix[0].x = zoom_x;
    matrix[1].y = zoom_y;
    matrix[2].z = (z_far + z_near) / (z_near - z_far);
    matrix[3].z = 2.0f * z_near * z_far / (z_near - z_far);
    matrix[2].w = -1.0f;
    return matrix;
}

void update_fov(Matrix4 &perspective_matrix, float aspect_ratio, float vertical_fov)
{
    calc_zoom_values(aspect_ratio, vertical_fov, perspective_matrix[0].x, perspective_matrix[1].y);
}

Matrix4 camera_matrix(Vector3 camera_pos, Vector3 target_pos, Vector3 world_up)
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
