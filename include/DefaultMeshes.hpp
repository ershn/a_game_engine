#pragma once

#include "Mesh.hpp"

namespace Age::Gfx
{
void create_plane_mesh(MeshBuffers &mesh_buffers, std::span<DrawCommand, 1> draw_commands);
void create_cube_mesh(MeshBuffers &mesh_buffers, std::span<DrawCommand, 1> draw_commands);
void create_cylinder_mesh(MeshBuffers &mesh_buffers, std::span<DrawCommand, 3> draw_commands, std::size_t side_count);

// TODO: add sphere mesh
// TODO: add capsule mesh

enum PrimitiveMesh : MeshId
{
    PLANE_MESH_ID,
    CUBE_MESH_ID,
    CYLINDER_MESH_ID
};

void load_primitive_meshes();

void create_null_mesh(MeshBuffers &mesh_buffers, std::span<DrawCommand, 1> draw_commands, std::size_t vertex_count);
} // namespace Age::Gfx
