#pragma once

#include "Matrix.hpp"
#include "Vector.hpp"

namespace Age::Math
{
Matrix4 scaling_matrix(Vector3 scaling);

Matrix4 x_rotation_matrix(float angle);
Matrix4 y_rotation_matrix(float angle);
Matrix4 z_rotation_matrix(float angle);

Matrix4 translation_matrix(Vector3 pos);

Matrix4 perspective_matrix(float z_near, float z_far, float aspect_ratio, float vertical_fov);
void update_fov(Matrix4 &perspective_matrix, float aspect_ratio, float vertical_fov);

Matrix4 camera_matrix(Vector3 camera_pos, Vector3 target_pos, Vector3 world_up);
} // namespace Age::Math
