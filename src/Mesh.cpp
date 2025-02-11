#include "Mesh.hpp"

namespace Age::Gfx
{
static std::vector<MeshBuffers> s_mesh_buffers{};
std::vector<DrawCommand> g_draw_commands{};
std::vector<Mesh> g_meshes{};

void init_mesh_system()
{
    s_mesh_buffers.reserve(128);
    g_draw_commands.reserve(256);
    g_meshes.reserve(128);
}

void create_elements_mesh(
    const Math::Vector3 *vertex_positions,
    const Math::Vector3 *vertex_colors,
    const Math::Vector3 *vertex_normals,
    std::size_t vertex_count,
    const unsigned short *vertex_indexes,
    std::size_t vertex_index_count,
    MeshBuffers &mesh_buffers,
    DrawCommand &draw_command
)
{
    std::size_t attr_array_size{vertex_count * sizeof(Math::Vector3)};

    glGenBuffers(1, &mesh_buffers.vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffers.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, attr_array_size * 3, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, attr_array_size, vertex_positions);
    glBufferSubData(GL_ARRAY_BUFFER, attr_array_size, attr_array_size, vertex_colors);
    glBufferSubData(GL_ARRAY_BUFFER, attr_array_size * 2, attr_array_size, vertex_normals);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::size_t index_array_size{vertex_index_count * sizeof(unsigned short)};

    glGenBuffers(1, &mesh_buffers.index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffers.index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_array_size, vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &mesh_buffers.vertex_array_object);
    glBindVertexArray(mesh_buffers.vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffers.vertex_buffer_object);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, reinterpret_cast<const void *>(attr_array_size));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, reinterpret_cast<const void *>(attr_array_size * 2));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffers.index_buffer_object);

    glBindVertexArray(0);

    draw_command.rendering_mode = GL_TRIANGLES;
    draw_command.element_count = static_cast<GLsizei>(vertex_index_count);
    draw_command.buffer_offset = 0;
}

MeshBuffers &create_mesh_buffers(std::uint16_t &index)
{
    index = static_cast<std::uint16_t>(s_mesh_buffers.size());
    return s_mesh_buffers.emplace_back();
}

MeshDrawCommands get_mesh_draw_commands(MeshId mesh_id)
{
    const Mesh &mesh{g_meshes[mesh_id]};
    return {
        .vertex_array_object = s_mesh_buffers[mesh.mesh_buffers_index].vertex_array_object,
        .draw_commands = {g_draw_commands.begin() + mesh.draw_command_offset, mesh.draw_command_count}
    };
}
} // namespace Age::Gfx
