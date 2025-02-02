#pragma once

#include "Matrix.hpp"
#include "Quaternion.hpp"
#include "Vector.hpp"

namespace Age::Math
{
Quaternion axis_angle_quaternion(const Vector3 &axis, float angle);

Matrix3 scaling_matrix(const Vector3 &scaling);
Matrix4 affine_scaling_matrix(const Vector3 &scaling);

Matrix3 rotation_matrix(const Quaternion &quat);
Matrix4 affine_rotation_matrix(const Quaternion &quat);
Matrix3 x_rotation_matrix(float angle);
Matrix3 y_rotation_matrix(float angle);
Matrix3 z_rotation_matrix(float angle);

Matrix4 translation_matrix(const Vector3 &pos);

Matrix4 perspective_matrix(float z_near, float z_far, float aspect_ratio, float vertical_fov);
void update_fov(Matrix4 &perspective_matrix, float aspect_ratio, float vertical_fov);

Matrix4 view_matrix(const Vector3 &camera_pos, const Vector3 &target_pos, const Vector3 &world_up);
} // namespace Age::Math
