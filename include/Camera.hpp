#pragma once

#include "ECS.hpp"
#include "Matrix.hpp"
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

struct Camera
{
    static constexpr auto TYPE{Core::ComponentType::CAMERA};

    float near_plane_z{};
    float far_plane_z{};
    float aspect_ratio{};
    float vertical_fov{};
};

struct ProjectionBufferBlockRef : public UniformBufferBlockRef<ProjectionBlock>
{
    static constexpr auto TYPE{Core::ComponentType::PROJECTION_BUFFER_BLOCK};

    using UniformBufferBlockRef<ProjectionBlock>::operator=;
};

void update_cameras_aspect_ratio(float aspect_ratio);
} // namespace Age::Gfx
