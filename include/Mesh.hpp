#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "ECS.hpp"
#include "OpenGL.hpp"

namespace Age::Gfx
{
struct MeshBuffers
{
    GLuint vertex_array_object{};
    GLuint vertex_buffer_object{};
    GLuint index_buffer_object{};
};

enum struct DrawCommandType : std::uint16_t
{
    DRAW_ARRAYS,
    DRAW_ELEMENTS,
};

struct DrawCommand
{
    DrawCommandType type{};
    OGL::RenderingMode rendering_mode{};
    std::uint32_t element_count{};
    std::size_t offset{};
};

using MeshId = std::uint16_t;

inline constexpr MeshId USER_MESH_START_ID{32};

struct Mesh
{
    std::uint32_t draw_command_offset{};
    std::uint16_t draw_command_count{};
    std::uint16_t mesh_buffers_index{};
};

struct MeshRef
{
    static constexpr auto TYPE{Core::ComponentType::MESH};

    MeshId mesh_id{};
};

struct MeshDrawCommands
{
    GLuint vertex_array_object{};
    std::span<const DrawCommand> draw_commands{};
};

extern std::vector<DrawCommand> g_draw_commands;
extern std::vector<Mesh> g_meshes;

void init_mesh_system();

void create_arrays_mesh(
    const Math::Vector3 *vertex_positions,
    const Math::Vector3 *vertex_colors,
    const Math::Vector3 *vertex_normals,
    const Math::Vector2 *vertex_texture_coords,
    std::size_t vertex_count,
    OGL::RenderingMode rendering_mode,
    MeshBuffers &mesh_buffers,
    DrawCommand &draw_command
);

void create_elements_mesh(
    const Math::Vector3 *vertex_positions,
    const Math::Vector3 *vertex_colors,
    const Math::Vector3 *vertex_normals,
    const Math::Vector2 *vertex_texture_coords,
    std::size_t vertex_count,
    const unsigned short *vertex_indices,
    std::size_t vertex_index_count,
    OGL::RenderingMode rendering_mode,
    MeshBuffers &mesh_buffers,
    DrawCommand &draw_command
);

MeshBuffers &create_mesh_buffers(std::uint16_t &index);

template <std::uint16_t Count>
std::span<DrawCommand, Count> create_draw_commands(std::uint32_t &offset)
{
    offset = static_cast<std::uint32_t>(g_draw_commands.size());
    g_draw_commands.resize(g_draw_commands.size() + Count);
    return std::span<DrawCommand, Count>{g_draw_commands.begin() + offset, Count};
}

template <std::uint16_t Count, typename... TArgs>
void create_mesh(
    MeshId mesh_id,
    std::function<void(MeshBuffers &, std::span<DrawCommand, Count>, TArgs...)> mesh_creator,
    TArgs &&...mesh_creator_args
)
{
    if (mesh_id >= g_meshes.size())
        g_meshes.resize(mesh_id + 1);

    Mesh &mesh{g_meshes[mesh_id]};
    MeshBuffers &mesh_buffers{create_mesh_buffers(mesh.mesh_buffers_index)};
    std::span<DrawCommand, Count> draw_commands{create_draw_commands<Count>(mesh.draw_command_offset)};
    mesh.draw_command_count = Count;

    mesh_creator(mesh_buffers, draw_commands, std::forward<TArgs>(mesh_creator_args)...);
}

const MeshBuffers &get_mesh_buffers(MeshId mesh_id);
MeshDrawCommands get_mesh_draw_commands(MeshId mesh_id);
} // namespace Age::Gfx
