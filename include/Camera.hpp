#pragma once

#include "Components.hpp"
#include "Matrix.hpp"
#include "Transform.hpp"
#include "UniformBuffer.hpp"
#include "Viewport.hpp"

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

inline constexpr unsigned int CLEAR_COLOR_BUFFER{0b1};
inline constexpr unsigned int CLEAR_DEPTH_BUFFER{0b10};
inline constexpr unsigned int DEPTH_CLAMPING{0b100};
inline constexpr unsigned int DEFAULT_CAMERA_FLAGS{CLEAR_COLOR_BUFFER | CLEAR_DEPTH_BUFFER};

struct CameraRenderState
{
    static constexpr auto TYPE{Core::ComponentType::CAMERA_RENDER_STATE};

    unsigned int flags{DEFAULT_CAMERA_FLAGS};
    Math::Vector4 clear_color{};
    float clear_depth{1.0f};
    ViewportId viewport_id{FULL_VIEWPORT_ID};
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

struct ProjectionBlock
{
    Math::Matrix4 view_to_clip_matrix{};
};

struct ProjectionUniformBuffer
{
    static constexpr auto TYPE{Core::ComponentType::PROJECTION_UNIFORM_BUFFER};

    UniformBuffer<ProjectionBlock> buffer;
    UniformBufferRangeId buffer_range_id{};
};

Math::Matrix4 window_space_orthographic_proj_matrix(int viewport_width, int viewport_height);
void update_window_space_orthographic_proj_size(
    Math::Matrix4 &orthographic_matrix, int viewport_width, int viewport_height
);

void update_perspective_camera_matrix(
    const CameraRenderState &camera_render_state,
    PerspectiveCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionUniformBuffer &projection_uniform_buffer
);

void update_orthographic_camera_matrix(
    const CameraRenderState &camera_render_state,
    OrthographicCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionUniformBuffer &projection_uniform_buffer
);

void update_window_space_camera_matrix(
    const CameraRenderState &camera_render_state,
    WindowSpaceCamera &camera,
    ViewToClipMatrix &view_to_clip_matrix,
    const ProjectionUniformBuffer &projection_uniform_buffer
);

void calc_camera_view_matrix(const Core::Transform &camera_transform, WorldToViewMatrix &view_matrix);
} // namespace Age::Gfx
