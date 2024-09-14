#include "Meshes.hpp"

namespace Age::Gfx
{
namespace
{
// clang-format off
const float plane_vertex_positions[] = {
    -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f
};

const float plane_vertex_colors[] = {
    0.859f, 0.533f, 0.416f,
    0.859f, 0.533f, 0.416f,
    0.859f, 0.533f, 0.416f,
    0.859f, 0.533f, 0.416f,
};

const GLushort plane_vertex_indices[] = {
    0, 1, 2,
    1, 3, 2
};

const float cube_vertex_positions[] = {
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f
};

const float cube_vertex_colors[] = {
    0.518f, 0.71f, 0.671f,
    0.518f, 0.71f, 0.671f,
    0.518f, 0.71f, 0.671f,
    0.518f, 0.71f, 0.671f,
    0.463f, 0.961f, 0.859f,
    0.463f, 0.961f, 0.859f,
    0.463f, 0.961f, 0.859f,
    0.463f, 0.961f, 0.859f,
};

const GLushort cube_vertex_indices[] = {
    0, 1, 2,
    1, 3, 2,
    1, 5, 3,
    5, 7, 3,
    5, 4, 7,
    4, 6, 7,
    4, 0, 6,
    0, 2, 6,
    4, 5, 0,
    5, 1, 0,
    2, 3, 6,
    3, 7, 6
};
// clang-format on
} // namespace

const Mesh &load_plane_mesh()
{
    static const Mesh *plane_mesh{};

    if (plane_mesh == nullptr)
    {
        plane_mesh =
            new Mesh{plane_vertex_positions, plane_vertex_colors,
                     sizeof(plane_vertex_positions) / (sizeof(float) * 3), plane_vertex_indices,
                     sizeof(plane_vertex_indices) / (sizeof(GLushort) * 3)};
    }
    return *plane_mesh;
}

const Mesh &load_cube_mesh()
{
    static const Mesh *cube_mesh{};

    if (cube_mesh == nullptr)
    {
        cube_mesh =
            new Mesh{cube_vertex_positions, cube_vertex_colors,
                     sizeof(cube_vertex_positions) / (sizeof(float) * 3), cube_vertex_indices,
                     sizeof(cube_vertex_indices) / (sizeof(GLushort) * 3)};
    }
    return *cube_mesh;
}
} // namespace Age::Gfx
