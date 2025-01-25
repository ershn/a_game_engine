#include "Mesh.hpp"

namespace Age::Gfx
{
static std::vector<MeshBuffer> s_mesh_buffers{};
std::vector<Mesh> g_meshes{};
std::vector<Model> g_models{};

void init_mesh_system()
{
    s_mesh_buffers.reserve(128);
    g_meshes.reserve(256);
    g_models.reserve(128);
}

MeshBuffer &create_mesh_buffer(std::uint16_t &index)
{
    index = static_cast<std::uint16_t>(s_mesh_buffers.size());
    return s_mesh_buffers.emplace_back();
}

DrawCommand get_draw_command(ModelId model_id)
{
    const Model &model{g_models[model_id]};
    return {.vertex_array_object = s_mesh_buffers[model.mesh_buffer_index].vertex_array_object,
            .meshes = {g_meshes.begin() + model.mesh_offset, model.mesh_count}};
}
} // namespace Age::Gfx
