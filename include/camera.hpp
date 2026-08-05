#pragma once

#include <cstdint>

#include "comparisons.hpp"
#include "components.hpp"
#include "framebuffer.hpp"
#include "matrix.hpp"
#include "transform.hpp"
#include "tuple.hpp"
#include "uniform_buffer.hpp"
#include "viewport.hpp"

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

using CameraStack = std::uint16_t;

struct CameraStackOrder
{
    static constexpr auto TYPE{Core::ComponentType::CAMERA_STACK_ORDER};

    CameraStack stack{};
    std::uint16_t order_in_stack{};
};

bool operator<(CameraStackOrder lhs, CameraStackOrder rhs);

enum struct Layer : std::uint8_t
{
};

inline constexpr unsigned int CLEAR_COLOR_BUFFER{0b1};
inline constexpr unsigned int CLEAR_DEPTH_BUFFER{0b10};
inline constexpr unsigned int DEFAULT_CAMERA_FLAGS{CLEAR_COLOR_BUFFER | CLEAR_DEPTH_BUFFER};

struct CameraRenderState
{
    static constexpr auto TYPE{Core::ComponentType::CAMERA_RENDER_STATE};

    unsigned int flags{DEFAULT_CAMERA_FLAGS};
    Math::Vector4 clear_color{};
    float clear_depth{1.0f};
    FramebufferId framebuffer_id{SYSTEM_FRAMEBUFFER_ID};
    ViewportId viewport_id{FULL_VIEWPORT_ID};
    Layer layer{};
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

template <typename TCameraIterator>
struct CameraStackIterator
{
    TCameraIterator camera_it{};
    TCameraIterator camera_end{};
    CameraStack camera_stack{};

    CameraStackIterator(TCameraIterator camera_it, TCameraIterator camera_end)
        : camera_it{camera_it}
        , camera_end{camera_end}
        , camera_stack{Util::get_ref<CameraStackOrder>(*camera_it).stack}
    {
    }

    operator bool() const
    {
        return camera_it != camera_end && Util::get_ref<CameraStackOrder>(*camera_it).stack == camera_stack;
    }

    CameraStackIterator &operator++()
    {
        ++camera_it;
        return *this;
    }

    auto &operator*() const
    {
        return *camera_it;
    }
};

template <Util::LessComparison TCameraStackCmp, typename TCameraIterator>
TCameraIterator for_each_camera_stack(
    CameraStack max_camera_stack, TCameraIterator camera_it, TCameraIterator camera_end, auto callback
)
{
    while (camera_it != camera_end &&
           TCameraStackCmp{}(Util::get_ref<CameraStackOrder>(*camera_it).stack, max_camera_stack))
    {
        CameraStackIterator camera_stack_it{camera_it, camera_end};

        callback(camera_stack_it);

        while (camera_stack_it)
            ++camera_stack_it;

        camera_it = camera_stack_it.camera_it;
    }

    return camera_it;
}

template <typename TCameraIterator>
void for_each_camera_stack(TCameraIterator camera_it, TCameraIterator camera_end, auto callback)
{
    while (camera_it != camera_end)
    {
        CameraStackIterator camera_stack_it{camera_it, camera_end};

        callback(camera_stack_it);

        while (camera_stack_it)
            ++camera_stack_it;

        camera_it = camera_stack_it.camera_it;
    }
}
} // namespace Age::Gfx
