#pragma once

#include "Mesh.hpp"

namespace Age::Gfx
{
void create_elements_mesh(const Math::Vector3 *vertex_positions, const Math::Vector3 *vertex_colors,
                          const Math::Vector3 *vertex_normals, std::size_t vertex_count,
                          const unsigned short *vertex_indexes, std::size_t vertex_index_count,
                          MeshBuffer &mesh_buffer, MeshElements &mesh_elements);

void create_plane_model(MeshBuffer &mesh_buffer, Model1MeshElements &model);
void create_cube_model(MeshBuffer &mesh_buffer, Model1MeshElements &model);
void create_cylinder_model(std::size_t side_count, MeshBuffer &mesh_buffer,
                           Model3MeshElements &model);
// TODO: add sphere model
// TODO: add capsule model

constexpr ModelId PLANE_MODEL_ID{0};
constexpr ModelId CUBE_MODEL_ID{1};
constexpr ModelId CYLINDER_MODEL_ID{2};

void load_primitive_models();

inline namespace Old
{
const Mesh &load_plane_mesh();
const Mesh &load_cube_mesh();
} // namespace Old
} // namespace Age::Gfx
