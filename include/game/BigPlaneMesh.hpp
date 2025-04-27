#pragma once

#include "Mesh.hpp"

namespace Game
{
void create_big_plane_mesh(Age::Gfx::MeshBuffers &mesh_buffers, std::span<Age::Gfx::DrawCommand, 1> draw_commands);
}
