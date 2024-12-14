#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

#include "glad/gl.h"

#include "Vector.hpp"

namespace Age::Gfx
{
class IMeshOld
{
  public:
    virtual ~IMeshOld() = default;

    virtual void draw() const = 0;
};

class Mesh : public IMeshOld
{
    GLuint _vertex_array_object{};
    GLuint _vertex_buffer_object{};
    GLuint _index_buffer_object{};
    std::size_t _primitive_count{};

  public:
    Mesh(const Math::Vector3 *vertex_positions, const Math::Vector3 *vertex_colors, const Math::Vector3 *vertex_normals,
         std::size_t vertex_count, const unsigned short *vertex_indices, std::size_t primitive_count);

    void draw() const override;
};

class CylinderMesh : public IMeshOld
{
    std::size_t _side_count{};
    GLuint _vertex_array_object{};
    GLuint _vertex_buffer_object{};
    GLuint _index_buffer_object{};

  public:
    CylinderMesh(const Math::Vector3 &color, std::size_t side_count);

    void draw() const override;
};

using ModelId = std::uint16_t;

enum class ModelType : std::uint8_t
{
    MESH_ELEMENTS_1,
    MESH_ELEMENTS_3
};

struct ModelIndex
{
    std::uint16_t mesh_buffer_index{};
    std::uint16_t model_index{};
    ModelType model_type{};
};

struct MeshBuffer
{
    GLuint vertex_array_object{};
    GLuint vertex_buffer_object{};
    GLuint index_buffer_object{};
};

struct MeshElements
{
    GLenum rendering_mode{};
    GLsizei element_count{};
    std::size_t buffer_offset{};
};

struct IModel
{
};

struct Model1MeshElements : public IModel
{
    MeshElements mesh_elements{};
};

struct Model3MeshElements : public IModel
{
    MeshElements mesh_elements[3]{};
};

void init_mesh_system();

void create_model(ModelId model_id, std::function<void(MeshBuffer &, Model1MeshElements &)> model_creator);
void create_model(ModelId model_id, std::function<void(MeshBuffer &, Model3MeshElements &)> model_creator);

void get_model(ModelId model_id, GLuint &vertex_array_object, ModelType &model_type, IModel &model);
} // namespace Age::Gfx
