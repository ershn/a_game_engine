#include "OpenGL.hpp"
#include "Vector.hpp"

#include "game/BigPlaneMesh.hpp"

namespace Game
{
using namespace Age;

namespace
{
// clang-format off
const Math::Vector3 s_positions[] = {
    {64.0f, 0.0f, -64.0f},
    {64.0f, 0.0f, 64.0f},
	{-64.0f, 0.0f, 64.0f},
	{-64.0f, 0.0f, -64.0f},
	{64.0f, 0.0f, -64.0f},
	{64.0f, 0.0f, 64.0f},
	{-64.0f, 0.0f, 64.0f},
	{-64.0f, 0.0f, -64.0f},
};

const Math::Vector2 s_texture_coords[] = {
    {64.0f, -64.0f},
    {64.0f, 64.0f},
	{-64.0f, 64.0f},
	{-64.0f, -64.0f},
	{64.0f, -64.0f},
	{64.0f, 64.0f},
	{-64.0f, 64.0f},
	{-64.0f, -64.0f},
};

const GLushort s_indexes[] = {
	0, 1, 2,
	2, 3, 0,
	4, 6, 5,
	6, 4, 7,
};
// clang-format on
} // namespace

void create_big_plane_mesh(Age::Gfx::MeshBuffers &mesh_buffers, std::span<Age::Gfx::DrawCommand, 1> draw_commands)
{
    Gfx::create_elements_mesh(
        s_positions,
        nullptr,
        nullptr,
        s_texture_coords,
        sizeof(s_positions) / sizeof(*s_positions),
        s_indexes,
        sizeof(s_indexes) / sizeof(*s_indexes),
        Gfx::OGL::RenderingMode::TRIANGLES,
        mesh_buffers,
        draw_commands[0]
    );
}
} // namespace Game
