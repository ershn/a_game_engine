#pragma once

#include <span>

#include "mesh.hpp"
#include "vector.hpp"

namespace Game
{
struct SphereImpostor
{
    Age::Math::Vector3 viewPosition;
    float radius{};
};

void create_sphere_impostors_mesh(
    Age::Gfx::MeshBuffers &mesh_buffer, std::span<Age::Gfx::DrawCommand, 1> draw_commands, std::size_t impostor_count
);
} // namespace Game
