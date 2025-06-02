#pragma once

#include "ECS.hpp"
#include "Matrix.hpp"
#include "Transform.hpp"
#include "UniformBlocks.hpp"
#include "UniformBuffer.hpp"

namespace Age::Gfx
{
struct WorldToViewMatrix
{
    static constexpr auto TYPE{Core::ComponentType::WORLD_TO_VIEW_MATRIX};

    Math::Matrix4 matrix{};
};

struct ViewToClipMatrix
{
    static constexpr auto TYPE{Core::ComponentType::VIEW_TO_CLIP_MATRIX};

    Math::Matrix4 matrix{};
};

struct PerspectiveCamera
{
    static constexpr auto TYPE{Core::ComponentType::PERSPECTIVE_CAMERA};

    float near_plane_z{};
    float far_plane_z{};
    float aspect_ratio{};
    float vertical_fov{};
};

struct OrthographicCamera
{
    static constexpr auto TYPE{Core::ComponentType::ORTHOGRAPHIC_CAMERA};

    float near_plane_z{};
    float far_plane_z{};
    float aspect_ratio{};
    float vertical_size{};
};

struct WindowSpaceCamera
{
    static constexpr auto TYPE{Core::ComponentType::WINDOW_SPACE_CAMERA};

    int viewport_width{};
    int viewport_height{};
};

struct ProjectionBufferBlockRef : public UniformBufferBlockRef<ProjectionBlock>
{
    static constexpr auto TYPE{Core::ComponentType::PROJECTION_BUFFER_BLOCK};

    using UniformBufferBlockRef<ProjectionBlock>::operator=;
};

Math::Matrix4 window_space_orthographic_proj_matrix(int viewport_width, int viewport_height);
void update_window_space_orthographic_proj_size(
    Math::Matrix4 &orthographic_matrix, int viewport_width, int viewport_height
);

void calc_camera_view_matrix(const Core::Transform &camera_transform, WorldToViewMatrix &view_matrix);

void update_camera_projections(int viewport_width, int viewport_height);
} // namespace Age::Gfx
