#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

#include "ECS.hpp"
#include "OpenGL.hpp"

namespace Age::Gfx
{
struct MeshBuffer
{
    GLuint vertex_array_object{};
    GLuint vertex_buffer_object{};
    GLuint index_buffer_object{};
};

struct Mesh
{
    GLenum rendering_mode{};
    GLsizei element_count{};
    std::size_t buffer_offset{};
};

using ModelId = std::uint16_t;

inline constexpr ModelId USER_MODEL_START_ID{32};

struct Model
{
    std::uint32_t mesh_offset{};
    std::uint16_t mesh_count{};
    std::uint16_t mesh_buffer_index{};
};

struct ModelRef
{
    static constexpr auto TYPE{Core::ComponentType::MODEL};

    ModelId model_id{};
};

struct DrawCommand
{
    GLuint vertex_array_object{};
    std::span<const Mesh> meshes{};
};

extern std::vector<Mesh> g_meshes;
extern std::vector<Model> g_models;

void init_mesh_system();

MeshBuffer &create_mesh_buffer(std::uint16_t &index);

template <std::uint16_t MeshCount>
std::span<Mesh, MeshCount> create_meshes(std::uint32_t &offset)
{
    offset = static_cast<std::uint32_t>(g_meshes.size());
    g_meshes.resize(g_meshes.size() + MeshCount);
    return std::span<Mesh, MeshCount>{g_meshes.begin() + offset, MeshCount};
}

template <std::uint16_t MeshCount>
void create_model(ModelId model_id, std::function<void(MeshBuffer &, std::span<Mesh, MeshCount>)> model_creator)
{
    if (model_id >= g_models.size())
        g_models.resize(model_id + 1);

    Model &model{g_models[model_id]};
    MeshBuffer &mesh_buffer{create_mesh_buffer(model.mesh_buffer_index)};
    std::span<Mesh, MeshCount> meshes{create_meshes<MeshCount>(model.mesh_offset)};
    model.mesh_count = MeshCount;

    model_creator(mesh_buffer, meshes);
}

DrawCommand get_draw_command(ModelId model_id);
} // namespace Age::Gfx
