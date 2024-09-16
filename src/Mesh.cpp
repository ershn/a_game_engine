#include <cmath>
#include <memory>
#include <numbers>

#include "Mesh.hpp"

namespace Age::Gfx
{
using Math::Vector3;

Mesh::Mesh(const Vector3 *vertex_positions, const Vector3 *vertex_colors,
           const Vector3 *vertex_normals, std::size_t vertex_count,
           const unsigned short *vertex_indices, std::size_t primitive_count)
    : _primitive_count{primitive_count}
{
    std::size_t attr_array_size{vertex_count * sizeof(Vector3)};

    glGenBuffers(1, &_vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, attr_array_size * 3, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, attr_array_size, vertex_positions);
    glBufferSubData(GL_ARRAY_BUFFER, attr_array_size, attr_array_size, vertex_colors);
    glBufferSubData(GL_ARRAY_BUFFER, attr_array_size * 2, attr_array_size, vertex_normals);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::size_t index_array_size{primitive_count * 3 * sizeof(unsigned short)};

    glGenBuffers(1, &_index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_array_size, vertex_indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_vertex_array_object);
    glBindVertexArray(_vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0,
                          reinterpret_cast<const void *>(attr_array_size));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, 0,
                          reinterpret_cast<const void *>(attr_array_size * 2));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object);

    glBindVertexArray(0);
}

void Mesh::draw() const
{
    glBindVertexArray(_vertex_array_object);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_primitive_count * 3), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

CylinderMesh::CylinderMesh(const Math::Vector3 &color, std::size_t side_count)
    : _side_count{side_count}
{
    std::size_t side_vertex_count{side_count * 2};
    std::size_t cap_vertex_count{side_count + 1};
    std::size_t vertex_count{side_vertex_count + cap_vertex_count * 2};
    std::size_t color_offset{vertex_count};
    std::size_t normal_offset{vertex_count * 2};
    std::size_t index_count{side_vertex_count + 2 + (cap_vertex_count + 1) * 2};
    auto vertex_buffer{std::make_unique<Vector3[]>(vertex_count * 3)};
    auto index_buffer{std::make_unique<GLushort[]>(index_count)};

    float angle_increment{std::numbers::pi_v<float> * 2.0f / side_count};
    for (std::size_t side_index{}; side_index < side_count; side_index++)
    {
        std::size_t top_vertex_index{side_index * 2};
        std::size_t bottom_vertex_index{top_vertex_index + 1};

        float angle{side_index * -angle_increment};
        Vector3 normal{std::sin(angle), 0.0f, std::cos(angle)};

        vertex_buffer[top_vertex_index] = normal * 0.5f + Vector3{0.0f, 0.5f, 0.0f};
        vertex_buffer[bottom_vertex_index] = normal * 0.5f + Vector3{0.0f, -0.5f, 0.0f};

        vertex_buffer[color_offset + top_vertex_index] = color;
        vertex_buffer[color_offset + bottom_vertex_index] = color;

        vertex_buffer[normal_offset + top_vertex_index] = normal;
        vertex_buffer[normal_offset + bottom_vertex_index] = normal;

        index_buffer[top_vertex_index] = static_cast<GLushort>(top_vertex_index);
        index_buffer[bottom_vertex_index] = static_cast<GLushort>(bottom_vertex_index);
    }
    index_buffer[side_vertex_count] = 0;
    index_buffer[side_vertex_count + 1] = 1;

    std::size_t top_cap_vertex_offset{side_vertex_count};
    std::size_t top_cap_color_offset{color_offset + side_vertex_count};
    std::size_t top_cap_normal_offset{normal_offset + side_vertex_count};
    std::size_t top_cap_index_offset{side_vertex_count + 2};

    vertex_buffer[top_cap_vertex_offset] = Vector3{0.0f, 0.5f, 0.0f};
    vertex_buffer[top_cap_color_offset] = color;
    vertex_buffer[top_cap_normal_offset] = Vector3{0.0f, 1.0f, 0.0f};
    index_buffer[top_cap_index_offset] = static_cast<GLushort>(top_cap_vertex_offset);

    for (std::size_t side_index{}; side_index < side_count; side_index++)
    {
        float angle{side_index * -angle_increment};
        Vector3 position{std::sin(angle) * 0.5f, 0.5f, std::cos(angle) * 0.5f};

        vertex_buffer[top_cap_vertex_offset + 1 + side_index] = position;
        vertex_buffer[top_cap_color_offset + 1 + side_index] = color;
        vertex_buffer[top_cap_normal_offset + 1 + side_index] = Vector3{0.0f, 1.0f, 0.0f};
        index_buffer[top_cap_index_offset + 1 + side_index] =
            static_cast<GLushort>(top_cap_vertex_offset + 1 + side_index);
    }
    index_buffer[top_cap_index_offset + 1 + side_count] =
        static_cast<GLushort>(top_cap_vertex_offset + 1);

    std::size_t bottom_cap_vertex_offset{top_cap_vertex_offset + 1 + side_count};
    std::size_t bottom_cap_color_offset{top_cap_color_offset + 1 + side_count};
    std::size_t bottom_cap_normal_offset{top_cap_normal_offset + 1 + side_count};
    std::size_t bottom_cap_index_offset{top_cap_index_offset + side_count + 2};

    vertex_buffer[bottom_cap_vertex_offset] = Vector3{0.0f, -0.5f, 0.0f};
    vertex_buffer[bottom_cap_color_offset] = color;
    vertex_buffer[bottom_cap_normal_offset] = Vector3{0.0f, -1.0f, 0.0f};
    index_buffer[bottom_cap_index_offset] = static_cast<GLushort>(bottom_cap_vertex_offset);

    for (std::size_t side_index{}; side_index < side_count; side_index++)
    {
        float angle{side_index * angle_increment};
        Vector3 position{std::sin(angle) * 0.5f, -0.5f, std::cos(angle) * 0.5f};

        vertex_buffer[bottom_cap_vertex_offset + 1 + side_index] = position;
        vertex_buffer[bottom_cap_color_offset + 1 + side_index] = color;
        vertex_buffer[bottom_cap_normal_offset + 1 + side_index] = Vector3{0.0f, -1.0f, 0.0f};
        index_buffer[bottom_cap_index_offset + 1 + side_index] =
            static_cast<GLushort>(bottom_cap_vertex_offset + 1 + side_index);
    }
    index_buffer[bottom_cap_index_offset + 1 + side_count] =
        static_cast<GLushort>(bottom_cap_vertex_offset + 1);

    glGenBuffers(1, &_vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(Vector3), vertex_buffer.get(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &_index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(GLushort), index_buffer.get(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_vertex_array_object);
    glBindVertexArray(_vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0,
                          reinterpret_cast<const GLvoid *>(color_offset * sizeof(Vector3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, 0,
                          reinterpret_cast<const GLvoid *>(normal_offset * sizeof(Vector3)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object);

    glBindVertexArray(0);
}

void CylinderMesh::draw() const
{
    glBindVertexArray(_vertex_array_object);
    glDrawElements(GL_TRIANGLE_FAN, static_cast<GLsizei>(_side_count + 2), GL_UNSIGNED_SHORT,
                   reinterpret_cast<const GLvoid *>(((_side_count + 1) * 2) * sizeof(GLushort)));
    glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>((_side_count + 1) * 2),
                   GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLE_FAN, static_cast<GLsizei>(_side_count + 2), GL_UNSIGNED_SHORT,
                   reinterpret_cast<const GLvoid *>(((_side_count + 1) * 2 + _side_count + 2) *
                                                    sizeof(GLushort)));
    glBindVertexArray(0);
}
} // namespace Age::Gfx
