#pragma once

#include "ECS.hpp"
#include "Matrix.hpp"
#include "UniformBlocks.hpp"
#include "UniformBuffer.hpp"

namespace Age::Gfx
{
struct WorldToCameraMatrix
{
    static constexpr auto TYPE{Core::ComponentType::WORLD_TO_CAMERA_MATRIX};

    Math::Matrix4 matrix{};
};

struct CameraToClipMatrix
{
    static constexpr auto TYPE{Core::ComponentType::CAMERA_TO_CLIP_MATRIX};

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

struct ProjectionBufferBlock : public UniformBufferBlock<ProjectionUniformBlock>
{
    static constexpr auto TYPE{Core::ComponentType::PROJECTION_BUFFER_BLOCK};

    using UniformBufferBlock<ProjectionUniformBlock>::operator=;
};

void update_cameras_aspect_ratio(float aspect_ratio);
} // namespace Age::Gfx
