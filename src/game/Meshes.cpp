#include "game/Meshes.hpp"

namespace Game
{
using namespace Age;

void create_sphere_impostors_mesh(
    Age::Gfx::MeshBuffers &mesh_buffer, std::span<Age::Gfx::DrawCommand, 1> draw_commands, std::size_t impostor_count
)
{
    glGenBuffers(1, &mesh_buffer.vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer.vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SphereImpostor) * impostor_count, nullptr, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &mesh_buffer.vertex_array_object);
    glBindVertexArray(mesh_buffer.vertex_array_object);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(SphereImpostor), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 1, GL_FLOAT, false, sizeof(SphereImpostor), reinterpret_cast<GLvoid *>(offsetof(SphereImpostor, radius))
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    draw_commands[0] = Gfx::DrawCommand{
        .type{Gfx::DrawCommandType::DRAW_ARRAYS},
        .rendering_mode{Gfx::OGL::RenderingMode::POINTS},
        .element_count{static_cast<std::uint32_t>(impostor_count)},
        .offset{0}
    };
}
} // namespace Game
