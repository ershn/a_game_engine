#pragma once

#include "Mesh.hpp"

namespace Age::Gfx
{
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
