#include "Meshes.hpp"

namespace Age::Gfx
{
using Math::Vector3;

namespace
{
// clang-format off
const Vector3 plane_vertex_positions[] = {
    {-0.5f,  0.5f, 0.0f},
    { 0.5f,  0.5f, 0.0f},
    {-0.5f, -0.5f, 0.0f},
    { 0.5f, -0.5f, 0.0f}
};

const Vector3 plane_vertex_colors[] = {
    {0.859f, 0.533f, 0.416f},
    {0.859f, 0.533f, 0.416f},
    {0.859f, 0.533f, 0.416f},
    {0.859f, 0.533f, 0.416f},
};

const Vector3 plane_vertex_normals[] = {
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
};

const GLushort plane_vertex_indices[] = {
    0, 1, 2,
    1, 3, 2
};

const Vector3 cube_vertex_positions[] = {
    {-0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},

    {-0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},

    {-0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},

    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},

    { 0.5f,  0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},

    {-0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
};

const Vector3 cube_vertex_colors[] = {
    {0.000f, 0.980f, 0.529f},
    {0.000f, 0.980f, 0.529f},
    {0.000f, 0.980f, 0.529f},
    {0.000f, 0.980f, 0.529f},

    {0.721f, 0.262f, 0.196f},
    {0.721f, 0.262f, 0.196f},
    {0.721f, 0.262f, 0.196f},
    {0.721f, 0.262f, 0.196f},

    {0.968f, 0.141f, 0.019f},
    {0.968f, 0.141f, 0.019f},
    {0.968f, 0.141f, 0.019f},
    {0.968f, 0.141f, 0.019f},

    {0.223f, 0.635f, 0.443f},
    {0.223f, 0.635f, 0.443f},
    {0.223f, 0.635f, 0.443f},
    {0.223f, 0.635f, 0.443f},

    {0.470f, 0.274f, 0.243f},
    {0.470f, 0.274f, 0.243f},
    {0.470f, 0.274f, 0.243f},
    {0.470f, 0.274f, 0.243f},

    {0.239f, 0.309f, 0.278f},
    {0.239f, 0.309f, 0.278f},
    {0.239f, 0.309f, 0.278f},
    {0.239f, 0.309f, 0.278f},
};

const Vector3 cube_vertex_normals[] = {
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},

    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},

    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},

    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},

    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},

    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
};

const GLushort cube_vertex_indices[] = {
    0, 1, 3,
    0, 3, 2,

    7, 5, 4,
    6, 7, 4,

    9, 8, 10,
    9, 10, 11,

    14, 12, 13,
    15, 14, 13,

    16, 18, 17,
    18, 19, 17,

    21, 22, 20,
    21, 23, 22,
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
                     plane_vertex_normals,   sizeof(plane_vertex_positions) / sizeof(Vector3),
                     plane_vertex_indices,   sizeof(plane_vertex_indices) / (sizeof(GLushort) * 3)};
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
                     cube_vertex_normals,   sizeof(cube_vertex_positions) / sizeof(Vector3),
                     cube_vertex_indices,   sizeof(cube_vertex_indices) / (sizeof(GLushort) * 3)};
    }
    return *cube_mesh;
}
} // namespace Age::Gfx
