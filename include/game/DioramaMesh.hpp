#pragma once

#include "Mesh.hpp"

namespace Game
{
void create_diorama_mesh(Age::Gfx::MeshBuffers &mesh_buffers, std::span<Age::Gfx::DrawCommand, 1> draw_commands);
}
