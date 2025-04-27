#pragma once

#include "Camera.hpp"
#include "Input.hpp"
#include "Math.hpp"
#include "SphericalCoord.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
struct SphericalCamera
{
    static constexpr auto TYPE{Core::ComponentType::SPHERICAL_CAMERA};

    float min_pitch{0.0f};
    float max_pitch{Math::PI};

    float min_distance{1.0f};
    float max_distance{100.0f};

    Math::Vector3 origin{};
    Math::SphericalCoord spherical_coord{};
};

void update_spherical_camera_via_input(const Input::MouseInput &mouse_input, SphericalCamera &spherical_camera);

void calc_spherical_camera_view_matrix(const SphericalCamera &spherical_camera, WorldToViewMatrix &view_matrix);
} // namespace Age::Gfx
