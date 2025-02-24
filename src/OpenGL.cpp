#include <iostream>

#include "OpenGL.hpp"

namespace Age::Gfx::OGL
{
GLuint create_shader(ShaderType shader_type)
{
    return glCreateShader(static_cast<GLenum>(shader_type));
}

void use_program(GLuint shader_program)
{
    glUseProgram(shader_program);
}

GLint get_uniform_location(GLuint shader_program, std::string_view name)
{
    GLint location{glGetUniformLocation(shader_program, name.data())};
    if (location == -1)
        std::cerr << "Uniform variable not found: " << name << '\n';
    return location;
}

void set_uniform(GLint location, float value)
{
    glUniform1f(location, value);
}

void set_uniform(GLint location, const Math::Vector3 &vector)
{
    glUniform3fv(location, 1, static_cast<const GLfloat *>(vector));
}

void set_uniform(GLint location, const Math::Vector4 &vector)
{
    glUniform4fv(location, 1, static_cast<const GLfloat *>(vector));
}

void set_uniform(GLint location, const Math::Matrix3 &matrix)
{
    glUniformMatrix3fv(location, 1, false, static_cast<const GLfloat *>(matrix));
}

void set_uniform(GLint location, const Math::Matrix4 &matrix)
{
    glUniformMatrix4fv(location, 1, false, static_cast<const GLfloat *>(matrix));
}

GLuint get_uniform_block_index(GLuint shader_program, std::string_view name)
{
    GLuint index{glGetUniformBlockIndex(shader_program, name.data())};
    if (index == GL_INVALID_INDEX)
        std::cerr << "Uniform block not found: " << name << '\n';
    return index;
}

void bind_uniform_block(GLuint shader_program, GLuint block_index, GLuint block_binding)
{
    glUniformBlockBinding(shader_program, block_index, block_binding);
}

void write_array_buffer(GLuint array_buffer_object, const void *data, std::size_t size)
{
    write_uniform_buffer(array_buffer_object, 0, data, size);
}

void write_array_buffer(GLuint array_buffer_object, std::size_t offset, const void *data, std::size_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer_object);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

GLuint create_uniform_buffer(std::size_t size)
{
    GLuint uniform_buffer_object;

    glGenBuffers(1, &uniform_buffer_object);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_object);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

    return uniform_buffer_object;
}

void write_uniform_buffer(GLuint uniform_buffer_object, const void *data, std::size_t size)
{
    write_uniform_buffer(uniform_buffer_object, 0, data, size);
}

void write_uniform_buffer(GLuint uniform_buffer_object, std::size_t offset, const void *data, std::size_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_object);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void bind_uniform_buffer_range(GLuint binding_point, GLuint uniform_buffer_object, std::size_t size)
{
    glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, uniform_buffer_object, 0, size);
}

void bind_uniform_buffer_range(GLuint binding_point, GLuint uniform_buffer_object, std::size_t offset, std::size_t size)
{
    glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, uniform_buffer_object, offset, size);
}

void bind_vertex_array_object(GLuint vao)
{
    glBindVertexArray(vao);
}

void set_clear_color(const Math::Vector4 &color)
{
    glClearColor(color.x, color.y, color.z, color.w);
}

void set_clear_depth(float depth)
{
    glClearDepth(depth);
}

void draw_arrays(RenderingMode rendering_mode, std::uint32_t element_count, std::size_t start_index)
{
    glDrawArrays(
        static_cast<GLenum>(rendering_mode), static_cast<GLint>(start_index), static_cast<GLsizei>(element_count)
    );
}

void draw_elements(RenderingMode rendering_mode, std::uint32_t element_count, std::size_t buffer_offset)
{
    glDrawElements(
        static_cast<GLenum>(rendering_mode),
        static_cast<GLsizei>(element_count),
        GL_UNSIGNED_SHORT,
        reinterpret_cast<const void *>(buffer_offset)
    );
}
} // namespace Age::Gfx::OGL
