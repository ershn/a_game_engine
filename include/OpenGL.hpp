#pragma once

#include <cstddef>
#include <string_view>

#include <glad/gl.h>

#include "Matrix.hpp"

namespace Age::Gfx
{
void use_program(GLuint shader_program);

GLint get_uniform_location(GLuint shader_program, std::string_view name);
void set_uniform(GLint location, float value);
void set_uniform(GLint location, const Math::Vector3 &vector);
void set_uniform(GLint location, const Math::Vector4 &vector);
void set_uniform(GLint location, const Math::Matrix3 &matrix);
void set_uniform(GLint location, const Math::Matrix4 &matrix);

GLuint get_uniform_block_index(GLuint shader_program, std::string_view name);
void bind_uniform_block(GLuint shader_program, GLuint block_index, GLuint block_binding);

void bind_vertex_array_object(GLuint vao);

void draw_elements(GLenum rendering_mode, GLsizei element_count, std::size_t buffer_offset);
} // namespace Age::Gfx
