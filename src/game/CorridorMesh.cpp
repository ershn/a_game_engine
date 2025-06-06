#include "OpenGL.hpp"
#include "Vector.hpp"

#include "game/CorridorMesh.hpp"

namespace Game
{
using namespace Age;

namespace
{
// clang-format off
const Math::Vector3 s_positions[] = {
    {1.0f, 0.0f, -64.0f},
	{1.0f, 0.0f, 64.0f},
	{-1.0f, 0.0f, 64.0f},
	{-1.0f, 0.0f, -64.0f},
	{-1.0f, 0.0f, -64.0f},
	{-1.0f, 0.0f, 64.0f},
	{-1.0f, 2.0f, 64.0f},
	{-1.0f, 2.0f, -64.0f},
	{1.0f, 0.0f, 64.0f},
	{1.0f, 0.0f, -64.0f},
	{1.0f, 2.0f, -64.0f},
	{1.0f, 2.0f, 64.0f},
	{1.0f, 2.0f, 64.0f},
	{1.0f, 2.0f, -64.0f},
	{-1.0f, 2.0f, -64.0f},
	{-1.0f, 2.0f, 64.0f},
};

const Math::Vector2 s_texture_coords[] = {
    {1.0f, -64.0f},
	{1.0f, 64.0f},
	{-1.0f, 64.0f},
	{-1.0f, -64.0f},
	{1.0f, -64.0f},
	{1.0f, 64.0f},
	{-1.0f, 64.0f},
	{-1.0f, -64.0f},
	{1.0f, -64.0f},
	{1.0f, 64.0f},
	{-1.0f, 64.0f},
	{-1.0f, -64.0f},
	{1.0f, -64.0f},
	{1.0f, 64.0f},
	{-1.0f, 64.0f},
	{-1.0f, -64.0f},
};

const GLushort s_indexes[] = {
    0, 1, 2,
    2, 3, 0,
    4, 5, 6,
    6, 7, 4,
    8, 9, 10,
    10, 11, 8,
    12, 13, 14,
    14, 15, 12,
};
// clang-format on
} // namespace

void create_corridor_mesh(Age::Gfx::MeshBuffers &mesh_buffers, std::span<Age::Gfx::DrawCommand, 1> draw_commands)
{
    Gfx::create_elements_mesh(
        s_positions,
        nullptr,
        nullptr,
        s_texture_coords,
        sizeof(s_positions) / sizeof(*s_positions),
        s_indexes,
        sizeof(s_indexes) / sizeof(*s_indexes),
        mesh_buffers,
        draw_commands[0]
    );
}
} // namespace Game
