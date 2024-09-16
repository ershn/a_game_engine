#pragma once

#include <cstddef>

#include "glad/gl.h"

#include "Vector.hpp"

namespace Age::Gfx
{
class IMesh
{
  public:
    virtual ~IMesh() = default;

    virtual void draw() const = 0;
};

class Mesh : public IMesh
{
    GLuint _vertex_array_object{};
    GLuint _vertex_buffer_object{};
    GLuint _index_buffer_object{};
    std::size_t _primitive_count{};

  public:
    Mesh(const Math::Vector3 *vertex_positions, const Math::Vector3 *vertex_colors,
         const Math::Vector3 *vertex_normals, std::size_t vertex_count,
         const unsigned short *vertex_indices, std::size_t primitive_count);

    void draw() const override;
};

class CylinderMesh : public IMesh
{
    std::size_t _side_count{};
    GLuint _vertex_array_object{};
    GLuint _vertex_buffer_object{};
    GLuint _index_buffer_object{};

  public:
    CylinderMesh(const Math::Vector3 &color, std::size_t side_count);

    void draw() const override;
};
} // namespace Age::Gfx
