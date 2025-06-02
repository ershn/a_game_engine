#include <cmath>

#include "Math.hpp"
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

template <typename TMatrix>
TMatrix x_rotation_matrix(float angle)
{
    TMatrix matrix{1.0f};
    float cosine{std::cos(angle)};
    float sinus{std::sin(angle)};
    matrix[1].y = cosine;
    matrix[1].z = sinus;
    matrix[2].y = -sinus;
    matrix[2].z = cosine;
    return matrix;
}

template <typename TMatrix>
TMatrix y_rotation_matrix(float angle)
{
    TMatrix matrix{1.0f};
    float cosine{std::cos(angle)};
    float sinus{std::sin(angle)};
    matrix[0].x = cosine;
    matrix[0].z = -sinus;
    matrix[2].x = sinus;
    matrix[2].z = cosine;
    return matrix;
}

template <typename TMatrix>
TMatrix z_rotation_matrix(float angle)
{
    TMatrix matrix{1.0f};
    float cosine{std::cos(angle)};
    float sinus{std::sin(angle)};
    matrix[0].x = cosine;
    matrix[0].y = sinus;
    matrix[1].x = -sinus;
    matrix[1].y = cosine;
    return matrix;
}

template <typename TMatrix>
TMatrix yx_rotation_matrix(float y_angle, float x_angle)
{
    TMatrix matrix{1.0f};
    float cos_y{std::cos(y_angle)};
    float sin_y{std::sin(y_angle)};
    float cos_x{std::cos(x_angle)};
    float sin_x{std::sin(x_angle)};
    matrix[0].x = cos_y;
    matrix[0].z = -sin_y;
    matrix[1].x = sin_y * sin_x;
    matrix[1].y = cos_x;
    matrix[1].z = cos_y * sin_x;
    matrix[2].x = sin_y * cos_x;
    matrix[2].y = -sin_x;
    matrix[2].z = cos_y * cos_x;
    return matrix;
}
} // namespace

Quaternion axis_angle_quaternion(const Vector3 &axis, float angle)
{
    Vector3 unit_axis{normalize(axis)};
    float half_angle{angle / 2.0f};
    float sin_half_angle{std::sin(half_angle)};

    return Quaternion{
        std::cos(half_angle), sin_half_angle * unit_axis.x, sin_half_angle * unit_axis.y, sin_half_angle * unit_axis.z
    };
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
    return x_rotation_matrix<Matrix3>(angle);
}

Matrix4 affine_x_rotation_matrix(float angle)
{
    return x_rotation_matrix<Matrix4>(angle);
}

// z to x positive angle
//
// ^x
// |
// +-->z
Matrix3 y_rotation_matrix(float angle)
{
    return y_rotation_matrix<Matrix3>(angle);
}

Matrix4 affine_y_rotation_matrix(float angle)
{
    return y_rotation_matrix<Matrix4>(angle);
}

// x to y positive angle
//
// ^y
// |
// +-->x
Matrix3 z_rotation_matrix(float angle)
{
    return z_rotation_matrix<Matrix3>(angle);
}

Matrix4 affine_z_rotation_matrix(float angle)
{
    return z_rotation_matrix<Matrix4>(angle);
}

// y_rotation(y_angle) * x_rotation(x_angle)
Matrix3 yx_rotation_matrix(float y_angle, float x_angle)
{
    return yx_rotation_matrix<Matrix3>(y_angle, x_angle);
}

// y_rotation(y_angle) * x_rotation(x_angle)
Matrix4 affine_yx_rotation_matrix(float y_angle, float x_angle)
{
    return yx_rotation_matrix<Matrix4>(y_angle, x_angle);
}

Matrix4 translation_matrix(const Vector3 &pos)
{
    Matrix4 matrix{1.0f};
    matrix[3] = Vector4{pos, 1.0f};
    return matrix;
}

static Vector2 calc_perspective_proj_zoom(float aspect_ratio, float vertical_fov)
{
    float proj_plane_vertical_size{tan(vertical_fov * 0.5f)};
    return Vector2{1.0f / (proj_plane_vertical_size * aspect_ratio), 1.0f / proj_plane_vertical_size};
}

Matrix4 perspective_proj_matrix(float z_near, float z_far, float aspect_ratio, float vertical_fov)
{
    Vector2 zoom{calc_perspective_proj_zoom(aspect_ratio, vertical_fov)};

    Matrix4 matrix{};
    matrix[0].x = zoom.x;
    matrix[1].y = zoom.y;
    matrix[2].z = (z_far + z_near) / (z_near - z_far);
    matrix[3].z = 2.0f * z_near * z_far / (z_near - z_far);
    matrix[2].w = -1.0f;
    return matrix;
}

void update_perspective_proj_zoom(Matrix4 &perspective_matrix, float aspect_ratio, float vertical_fov)
{
    Vector2 zoom{calc_perspective_proj_zoom(aspect_ratio, vertical_fov)};

    perspective_matrix[0].x = zoom.x;
    perspective_matrix[1].y = zoom.y;
}

static Vector2 calc_orthographic_proj_size(float aspect_ratio, float vertical_size)
{
    return Vector2{1.0f / (vertical_size * 0.5f * aspect_ratio), 1.0f / (vertical_size * 0.5f)};
}

Matrix4 orthographic_proj_matrix(float z_near, float z_far, float aspect_ratio, float vertical_size)
{
    Vector2 orthographic_size{calc_orthographic_proj_size(aspect_ratio, vertical_size)};

    Matrix4 matrix{};
    matrix[0].x = orthographic_size.x;
    matrix[1].y = orthographic_size.y;
    matrix[2].z = 2.0f / (z_near - z_far);
    matrix[3].z = 2.0f * z_near / (z_near - z_far) - 1.0f;
    matrix[3].w = 1.0f;
    return matrix;
}

void update_orthographic_proj_size(Matrix4 &orthographic_matrix, float aspect_ratio, float vertical_size)
{
    Vector2 orthographic_size{calc_orthographic_proj_size(aspect_ratio, vertical_size)};

    orthographic_matrix[0].x = orthographic_size.x;
    orthographic_matrix[1].y = orthographic_size.y;
}

Matrix4 look_at_matrix(const Vector3 &target_pos, const Vector3 &camera_pos, const Vector3 &world_up)
{
    Matrix4 matrix{1.0f};

    Vector3 forward{normalize(camera_pos - target_pos)};
    Vector3 right{normalize(cross(world_up, forward))};
    Vector3 up{cross(forward, right)};

    matrix[0].x = right.x;
    matrix[1].x = right.y;
    matrix[2].x = right.z;
    matrix[0].y = up.x;
    matrix[1].y = up.y;
    matrix[2].y = up.z;
    matrix[0].z = forward.x;
    matrix[1].z = forward.y;
    matrix[2].z = forward.z;

    matrix *= translation_matrix(-camera_pos);
    return matrix;
}

Matrix4 look_at_matrix(const Vector3 &target_pos, const SphericalCoord &camera_coord)
{
    Matrix4 matrix{affine_yx_rotation_matrix(camera_coord.angles.y, camera_coord.angles.x - PI * 0.5f)};
    Vector3 position{target_pos + xyz(matrix[2] * camera_coord.distance)};
    matrix.transpose();
    matrix *= translation_matrix(-position);
    return matrix;
}
} // namespace Age::Math
