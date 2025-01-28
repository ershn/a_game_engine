#pragma once

#include "Mesh.hpp"

namespace Game
{
void create_ground_mesh(Age::Gfx::MeshBuffer &mesh_buffer, std::span<Age::Gfx::Mesh, 1> meshes);
}
