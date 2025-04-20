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
    const Math::Vector2 *vertex_texture_coords,
    std::size_t vertex_count,
    const unsigned short *vertex_indexes,
    std::size_t vertex_index_count,
    MeshBuffers &mesh_buffers,
    DrawCommand &draw_command
)
{
    glGenVertexArrays(1, &mesh_buffers.vertex_array_object);
    glBindVertexArray(mesh_buffers.vertex_array_object);

    std::size_t buffer_size{vertex_count * sizeof(Math::Vector3)};
    if (vertex_colors != nullptr)
        buffer_size += vertex_count * sizeof(Math::Vector3);
    if (vertex_normals != nullptr)
        buffer_size += vertex_count * sizeof(Math::Vector3);
    if (vertex_texture_coords != nullptr)
        buffer_size += vertex_count * sizeof(Math::Vector2);

    glGenBuffers(1, &mesh_buffers.vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffers.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

    GLuint attr_index{0};
    std::size_t buffer_offset{0};

    glEnableVertexAttribArray(attr_index);
    glVertexAttribPointer(attr_index, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
    glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(Math::Vector3), vertex_positions);
    ++attr_index;
    buffer_offset += vertex_count * sizeof(Math::Vector3);

    if (vertex_colors != nullptr)
    {
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(Math::Vector3), vertex_colors);
        ++attr_index;
        buffer_offset += vertex_count * sizeof(Math::Vector3);
    }
    if (vertex_normals != nullptr)
    {
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(Math::Vector3), vertex_normals);
        ++attr_index;
        buffer_offset += vertex_count * sizeof(Math::Vector3);
    }
    if (vertex_texture_coords != nullptr)
    {
        glEnableVertexAttribArray(attr_index);
        glVertexAttribPointer(attr_index, 2, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(Math::Vector2), vertex_texture_coords);
        ++attr_index;
        buffer_offset += vertex_count * sizeof(Math::Vector2);
    }

    glGenBuffers(1, &mesh_buffers.index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffers.index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_index_count * sizeof(unsigned short), vertex_indexes, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    draw_command.type = DrawCommandType::DRAW_ELEMENTS;
    draw_command.rendering_mode = OGL::RenderingMode::TRIANGLES;
    draw_command.element_count = static_cast<std::uint32_t>(vertex_index_count);
    draw_command.offset = 0;
}

MeshBuffers &create_mesh_buffers(std::uint16_t &index)
{
    index = static_cast<std::uint16_t>(s_mesh_buffers.size());
    return s_mesh_buffers.emplace_back();
}

const MeshBuffers &get_mesh_buffers(MeshId mesh_id)
{
    const Mesh &mesh{g_meshes[mesh_id]};
    return s_mesh_buffers[mesh.mesh_buffers_index];
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
