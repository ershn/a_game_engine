#include <functional>
#include <numbers>

#include "MeshInstances.hpp"

namespace Age::Gfx
{
using Math::Vector3;

namespace
{
// clang-format off
const Vector3 s_plane_vertex_positions[] = {
    {-0.5f,  0.5f, 0.0f},
    { 0.5f,  0.5f, 0.0f},
    {-0.5f, -0.5f, 0.0f},
    { 0.5f, -0.5f, 0.0f},
};

const Vector3 s_plane_vertex_colors[] = {
    {0.859f, 0.533f, 0.416f},
    {0.859f, 0.533f, 0.416f},
    {0.859f, 0.533f, 0.416f},
    {0.859f, 0.533f, 0.416f},
};

const Vector3 s_plane_vertex_normals[] = {
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
};

const GLushort s_plane_vertex_indices[] = {
    0, 1, 2,
    1, 3, 2,
};

const Vector3 s_cube_vertex_positions[] = {
    {-0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},

    {-0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},

    {-0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},

    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},

    { 0.5f,  0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},

    {-0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
};

const Vector3 s_cube_vertex_colors[] = {
    {0.000f, 0.980f, 0.529f},
    {0.000f, 0.980f, 0.529f},
    {0.000f, 0.980f, 0.529f},
    {0.000f, 0.980f, 0.529f},

    {0.721f, 0.262f, 0.196f},
    {0.721f, 0.262f, 0.196f},
    {0.721f, 0.262f, 0.196f},
    {0.721f, 0.262f, 0.196f},

    {0.968f, 0.141f, 0.019f},
    {0.968f, 0.141f, 0.019f},
    {0.968f, 0.141f, 0.019f},
    {0.968f, 0.141f, 0.019f},

    {0.223f, 0.635f, 0.443f},
    {0.223f, 0.635f, 0.443f},
    {0.223f, 0.635f, 0.443f},
    {0.223f, 0.635f, 0.443f},

    {0.470f, 0.274f, 0.243f},
    {0.470f, 0.274f, 0.243f},
    {0.470f, 0.274f, 0.243f},
    {0.470f, 0.274f, 0.243f},

    {0.239f, 0.309f, 0.278f},
    {0.239f, 0.309f, 0.278f},
    {0.239f, 0.309f, 0.278f},
    {0.239f, 0.309f, 0.278f},
};

const Vector3 s_cube_vertex_normals[] = {
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},
    { 0.0f,  0.0f,  1.0f},

    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f, -1.0f},

    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  1.0f,  0.0f},

    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},

    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},

    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
    {-1.0f,  0.0f,  0.0f},
};

const GLushort s_cube_vertex_indices[] = {
    0, 1, 3,
    0, 3, 2,

    7, 5, 4,
    6, 7, 4,

    9, 8, 10,
    9, 10, 11,

    14, 12, 13,
    15, 14, 13,

    16, 18, 17,
    18, 19, 17,

    21, 22, 20,
    21, 23, 22,
};
// clang-format on
} // namespace

void create_plane_model(MeshBuffer &mesh_buffer, std::span<Mesh, 1> meshes)
{
    create_elements_mesh(s_plane_vertex_positions, s_plane_vertex_colors, s_plane_vertex_normals,
                         sizeof(s_plane_vertex_positions) / sizeof(Vector3), s_plane_vertex_indices,
                         sizeof(s_plane_vertex_indices) / sizeof(GLushort), mesh_buffer, meshes[0]);
}

void create_cube_model(MeshBuffer &mesh_buffer, std::span<Mesh, 1> meshes)
{
    create_elements_mesh(s_cube_vertex_positions, s_cube_vertex_colors, s_cube_vertex_normals,
                         sizeof(s_cube_vertex_positions) / sizeof(Vector3), s_cube_vertex_indices,
                         sizeof(s_cube_vertex_indices) / sizeof(GLushort), mesh_buffer, meshes[0]);
}

void create_cylinder_model(std::size_t side_count, MeshBuffer &mesh_buffer, std::span<Mesh, 3> meshes)
{
    const Vector3 color{1.0f, 1.0f, 1.0f};

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
    index_buffer[top_cap_index_offset + 1 + side_count] = static_cast<GLushort>(top_cap_vertex_offset + 1);

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
    index_buffer[bottom_cap_index_offset + 1 + side_count] = static_cast<GLushort>(bottom_cap_vertex_offset + 1);

    glGenBuffers(1, &mesh_buffer.vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(Vector3), vertex_buffer.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mesh_buffer.index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffer.index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(GLushort), index_buffer.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &mesh_buffer.vertex_array_object);
    glBindVertexArray(mesh_buffer.vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer.vertex_buffer_object);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, reinterpret_cast<const GLvoid *>(color_offset * sizeof(Vector3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, reinterpret_cast<const GLvoid *>(normal_offset * sizeof(Vector3)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffer.index_buffer_object);

    glBindVertexArray(0);

    meshes[0] = Mesh{.rendering_mode = GL_TRIANGLE_STRIP,
                     .element_count = static_cast<GLsizei>((side_count + 1) * 2),
                     .buffer_offset = 0};
    meshes[1] = Mesh{.rendering_mode = GL_TRIANGLE_FAN,
                     .element_count = static_cast<GLsizei>(side_count + 2),
                     .buffer_offset = (side_count + 1) * 2 * sizeof(GLushort)};
    meshes[2] = Mesh{.rendering_mode = GL_TRIANGLE_FAN,
                     .element_count = static_cast<GLsizei>(side_count + 2),
                     .buffer_offset = ((side_count + 1) * 2 + side_count + 2) * sizeof(GLushort)};
}

void load_primitive_models()
{
    create_model<1>(PLANE_MODEL_ID, create_plane_model);
    create_model<1>(CUBE_MODEL_ID, create_cube_model);
    create_model<3>(CYLINDER_MODEL_ID, std::bind_front(create_cylinder_model, 30));
}
} // namespace Age::Gfx
