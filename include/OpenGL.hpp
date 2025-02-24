#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include <glad/gl.h>

#include "Matrix.hpp"

namespace Age::Gfx::OGL
{
enum struct ShaderType : std::uint16_t
{
    VERTEX = GL_VERTEX_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER
};

enum struct RenderingMode : std::uint16_t
{
    POINTS = GL_POINTS,
    LINES = GL_LINES,
    LINE_LOOP = GL_LINE_LOOP,
    LINE_STRIP = GL_LINE_STRIP,
    TRIANGLES = GL_TRIANGLES,
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    TRIANGLE_FAN = GL_TRIANGLE_FAN,
    LINES_ADJACENCY = GL_LINES_ADJACENCY,
    LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
    TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
    TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
};

GLuint create_shader(ShaderType shader_type);

void use_program(GLuint shader_program);

GLint get_uniform_location(GLuint shader_program, std::string_view name);
void set_uniform(GLint location, float value);
void set_uniform(GLint location, const Math::Vector3 &vector);
void set_uniform(GLint location, const Math::Vector4 &vector);
void set_uniform(GLint location, const Math::Matrix3 &matrix);
void set_uniform(GLint location, const Math::Matrix4 &matrix);

GLuint get_uniform_block_index(GLuint shader_program, std::string_view name);
void bind_uniform_block(GLuint shader_program, GLuint block_index, GLuint block_binding);

void write_array_buffer(GLuint array_buffer_object, const void *data, std::size_t size);
void write_array_buffer(GLuint array_buffer_object, std::size_t offset, const void *data, std::size_t size);

GLuint create_uniform_buffer(std::size_t size);
void write_uniform_buffer(GLuint uniform_buffer_object, const void *data, std::size_t size);
void write_uniform_buffer(GLuint uniform_buffer_object, std::size_t offset, const void *data, std::size_t size);
void bind_uniform_buffer_range(GLuint binding_point, GLuint uniform_buffer_object, std::size_t size);
void bind_uniform_buffer_range(
    GLuint binding_point, GLuint uniform_buffer_object, std::size_t offset, std::size_t size
);

void bind_vertex_array_object(GLuint vao);

void set_clear_color(const Math::Vector4 &color);
void set_clear_depth(float depth);

void draw_arrays(RenderingMode rendering_mode, std::uint32_t element_count, std::size_t start_index);
void draw_elements(RenderingMode rendering_mode, std::uint32_t element_count, std::size_t buffer_offset);
} // namespace Age::Gfx::OGL
