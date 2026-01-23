#include "Mesh.hpp"

namespace Age::Gfx
{
namespace
{
constexpr GLenum s_vertex_attr_type_to_gl_enum[] = {
    GL_BYTE,
    GL_UNSIGNED_BYTE,
    GL_SHORT,
    GL_UNSIGNED_SHORT,
    GL_INT,
    GL_UNSIGNED_INT,
    GL_INT_2_10_10_10_REV,
    GL_UNSIGNED_INT_2_10_10_10_REV,
    GL_FLOAT,
    GL_DOUBLE,
};

constexpr GLenum to_gl_enum(VertexAttrType vertex_attr_type)
{
    return s_vertex_attr_type_to_gl_enum[static_cast<std::size_t>(vertex_attr_type)];
}

void set_vertex_attr(std::uint8_t index, VertexAttrDesc desc)
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(
        index,
        desc.size,
        to_gl_enum(desc.type),
        desc.normalized,
        desc.stride,
        reinterpret_cast<void *>(static_cast<std::uintptr_t>(desc.offset))
    );
}

std::vector<MeshBuffers> s_mesh_buffers{};
} // namespace

std::vector<DrawCommand> g_draw_commands{};
std::vector<Mesh> g_meshes{};

void init_mesh_system()
{
    s_mesh_buffers.reserve(128);
    g_draw_commands.reserve(256);
    g_meshes.reserve(128);
}

MeshBuffers &create_mesh_buffers(std::uint16_t &index)
{
    index = static_cast<std::uint16_t>(s_mesh_buffers.size());
    return s_mesh_buffers.emplace_back();
}

void create_arrays_mesh(
    VertexAttrDesc vertex_positions_desc,
    VertexAttrDesc vertex_colors_desc,
    VertexAttrDesc vertex_normals_desc,
    VertexAttrDesc vertex_texture_coords_desc,
    std::span<const std::byte> vertex_data,
    std::size_t vertex_count,
    OGL::RenderingMode rendering_mode,
    MeshBuffers &mesh_buffers,
    DrawCommand &draw_command
)
{
    glGenVertexArrays(1, &mesh_buffers.vertex_array_object);
    glBindVertexArray(mesh_buffers.vertex_array_object);

    glGenBuffers(1, &mesh_buffers.vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffers.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);

    if (vertex_positions_desc.size > 0)
        set_vertex_attr(0, vertex_positions_desc);
    if (vertex_colors_desc.size > 0)
        set_vertex_attr(1, vertex_colors_desc);
    if (vertex_normals_desc.size > 0)
        set_vertex_attr(2, vertex_normals_desc);
    if (vertex_texture_coords_desc.size > 0)
        set_vertex_attr(3, vertex_texture_coords_desc);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    draw_command = {
        .type = DrawCommandType::DRAW_ARRAYS,
        .rendering_mode = rendering_mode,
        .element_count = static_cast<std::uint32_t>(vertex_count),
        .offset = 0
    };
}

void create_elements_mesh(
    VertexAttrDesc vertex_positions_desc,
    VertexAttrDesc vertex_colors_desc,
    VertexAttrDesc vertex_normals_desc,
    VertexAttrDesc vertex_texture_coords_desc,
    std::span<const std::byte> vertex_data,
    std::span<const std::uint16_t> vertex_indices,
    OGL::RenderingMode rendering_mode,
    MeshBuffers &mesh_buffers,
    DrawCommand &draw_command
)
{
    glGenVertexArrays(1, &mesh_buffers.vertex_array_object);
    glBindVertexArray(mesh_buffers.vertex_array_object);

    glGenBuffers(1, &mesh_buffers.vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffers.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);

    if (vertex_positions_desc.size > 0)
        set_vertex_attr(0, vertex_positions_desc);
    if (vertex_colors_desc.size > 0)
        set_vertex_attr(1, vertex_colors_desc);
    if (vertex_normals_desc.size > 0)
        set_vertex_attr(2, vertex_normals_desc);
    if (vertex_texture_coords_desc.size > 0)
        set_vertex_attr(3, vertex_texture_coords_desc);

    glGenBuffers(1, &mesh_buffers.index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffers.index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size_bytes(), vertex_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    draw_command = {
        .type = DrawCommandType::DRAW_ELEMENTS,
        .rendering_mode = rendering_mode,
        .element_count = static_cast<std::uint32_t>(vertex_indices.size()),
        .offset = 0
    };
}

void create_arrays_mesh(
    const Math::Vector3 *vertex_positions,
    const Math::Vector3 *vertex_colors,
    const Math::Vector3 *vertex_normals,
    const Math::Vector2 *vertex_texture_coords,
    std::size_t vertex_count,
    OGL::RenderingMode rendering_mode,
    MeshBuffers &mesh_buffers,
    DrawCommand &draw_command
)
{
    glGenVertexArrays(1, &mesh_buffers.vertex_array_object);
    glBindVertexArray(mesh_buffers.vertex_array_object);

    std::size_t buffer_size{vertex_count * sizeof(*vertex_positions)};
    if (vertex_colors != nullptr)
        buffer_size += vertex_count * sizeof(*vertex_colors);
    if (vertex_normals != nullptr)
        buffer_size += vertex_count * sizeof(*vertex_normals);
    if (vertex_texture_coords != nullptr)
        buffer_size += vertex_count * sizeof(*vertex_texture_coords);

    glGenBuffers(1, &mesh_buffers.vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffers.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);

    std::size_t buffer_offset{0};

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
    glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(*vertex_positions), vertex_positions);
    buffer_offset += vertex_count * sizeof(*vertex_positions);

    if (vertex_colors != nullptr)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(*vertex_colors), vertex_colors);
        buffer_offset += vertex_count * sizeof(*vertex_colors);
    }
    if (vertex_normals != nullptr)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(*vertex_normals), vertex_normals);
        buffer_offset += vertex_count * sizeof(*vertex_normals);
    }
    if (vertex_texture_coords != nullptr)
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(
            GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(*vertex_texture_coords), vertex_texture_coords
        );
        buffer_offset += vertex_count * sizeof(*vertex_texture_coords);
    }

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    draw_command.type = DrawCommandType::DRAW_ARRAYS;
    draw_command.rendering_mode = rendering_mode;
    draw_command.element_count = static_cast<std::uint32_t>(vertex_count);
    draw_command.offset = 0;
}

void create_elements_mesh(
    const Math::Vector3 *vertex_positions,
    const Math::Vector3 *vertex_colors,
    const Math::Vector3 *vertex_normals,
    const Math::Vector2 *vertex_texture_coords,
    std::size_t vertex_count,
    std::span<const std::uint16_t> vertex_indices,
    OGL::RenderingMode rendering_mode,
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

    std::size_t buffer_offset{0};

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
    glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(Math::Vector3), vertex_positions);
    buffer_offset += vertex_count * sizeof(Math::Vector3);

    if (vertex_colors != nullptr)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(Math::Vector3), vertex_colors);
        buffer_offset += vertex_count * sizeof(Math::Vector3);
    }
    if (vertex_normals != nullptr)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(Math::Vector3), vertex_normals);
        buffer_offset += vertex_count * sizeof(Math::Vector3);
    }
    if (vertex_texture_coords != nullptr)
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, false, 0, reinterpret_cast<GLvoid *>(buffer_offset));
        glBufferSubData(GL_ARRAY_BUFFER, buffer_offset, vertex_count * sizeof(Math::Vector2), vertex_texture_coords);
        buffer_offset += vertex_count * sizeof(Math::Vector2);
    }

    glGenBuffers(1, &mesh_buffers.index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffers.index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size_bytes(), vertex_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    draw_command.type = DrawCommandType::DRAW_ELEMENTS;
    draw_command.rendering_mode = rendering_mode;
    draw_command.element_count = static_cast<std::uint32_t>(vertex_indices.size());
    draw_command.offset = 0;
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
