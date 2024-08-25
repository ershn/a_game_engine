#include <cmath>
#include <memory>
#include <numbers>

#include "Mesh.hpp"
#include "Vector.hpp"

namespace Age::Gfx
{
Mesh::Mesh(const float *vertex_positions, const float *vertex_colors, std::size_t vertex_count,
           const unsigned short *vertex_indices, std::size_t primitive_count)
    : _primitive_count{primitive_count}
{
    std::size_t attr_array_size{vertex_count * 3 * sizeof(float)};

    glGenBuffers(1, &_vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, attr_array_size * 2, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, attr_array_size, vertex_positions);
    glBufferSubData(GL_ARRAY_BUFFER, attr_array_size, attr_array_size, vertex_colors);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object);

    glBindVertexArray(0);
}

void Mesh::draw() const
{
    glBindVertexArray(_vertex_array_object);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_primitive_count * 3), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

CylinderMesh::CylinderMesh(std::size_t side_count)
    : _side_count{side_count}
{
    using Math::Vector3;

    std::size_t side_vertex_count{side_count * 2};
    std::size_t vertex_count{side_vertex_count + 2};
    std::size_t index_count{side_vertex_count + 2 + (side_count + 2) * 2};
    auto vertex_buffer{std::make_unique<Vector3[]>(vertex_count)};
    auto index_buffer{std::make_unique<GLushort[]>(index_count)};

    vertex_buffer[0] = Vector3{0.0f, 0.5f, 0.0f};
    vertex_buffer[1] = Vector3{0.0f, -0.5f, 0.0f};

    float angle_increment{std::numbers::pi_v<float> * 2.0f / side_count};
    for (std::size_t side_index{}; side_index < side_count; side_index++)
    {
        float angle{side_index * -angle_increment};
        float z{std::cos(angle) * 0.5f};
        float x{std::sin(angle) * 0.5f};

        vertex_buffer[2 + side_index * 2] = Vector3{x, 0.5f, z};
        vertex_buffer[2 + side_index * 2 + 1] = Vector3{x, -0.5f, z};

        index_buffer[side_index * 2] = static_cast<GLushort>(2 + side_index * 2);
        index_buffer[side_index * 2 + 1] = static_cast<GLushort>(2 + side_index * 2 + 1);
    }
    index_buffer[side_count * 2] = 2;
    index_buffer[side_count * 2 + 1] = 3;

    std::size_t top_fan_index_offset{side_count * 2 + 2};
    std::size_t bottom_fan_index_offset{top_fan_index_offset + side_count + 2};
    index_buffer[top_fan_index_offset] = 0;
    index_buffer[bottom_fan_index_offset] = 1;
    for (std::size_t side_index{}; side_index < side_count; side_index++)
    {
        index_buffer[top_fan_index_offset + 1 + side_index] =
            static_cast<GLushort>(2 + side_index * 2);
        index_buffer[bottom_fan_index_offset + 1 + side_index] =
            static_cast<GLushort>(3 + (side_count - 1 - side_index) * 2);
    }
    index_buffer[top_fan_index_offset + 1 + side_count] = 2;
    index_buffer[bottom_fan_index_offset + 1 + side_count] =
        static_cast<GLushort>(3 + (side_count - 1) * 2);

    glGenBuffers(1, &_vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vector3), vertex_buffer.get(),
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