#pragma once

#include "Mesh.hpp"

namespace Game
{
void create_infinity_symbol_mesh(Age::Gfx::MeshBuffers &mesh_buffer, std::span<Age::Gfx::DrawCommand, 1> draw_commands);
}
