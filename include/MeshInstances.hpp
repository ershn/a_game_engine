#pragma once

#include "Mesh.hpp"

namespace Age::Gfx
{
void create_elements_mesh(const Math::Vector3 *vertex_positions,
                          const Math::Vector3 *vertex_colors,
                          const Math::Vector3 *vertex_normals,
                          std::size_t vertex_count,
                          const unsigned short *vertex_indexes,
                          std::size_t vertex_index_count,
                          MeshBuffer &mesh_buffer,
                          Mesh &mesh_elements);

void create_plane_model(MeshBuffer &mesh_buffer, std::span<Mesh, 1> meshes);
void create_cube_model(MeshBuffer &mesh_buffer, std::span<Mesh, 1> meshes);
void create_cylinder_model(std::size_t side_count, MeshBuffer &mesh_buffer, std::span<Mesh, 3> meshes);

// TODO: add sphere model
// TODO: add capsule model

enum PrimitiveModel : ModelId
{
    PLANE_MODEL_ID,
    CUBE_MODEL_ID,
    CYLINDER_MODEL_ID
};

void load_primitive_models();
} // namespace Age::Gfx
