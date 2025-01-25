#include <iostream>

#include "OpenGL.hpp"

namespace Age::Gfx
{
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

void bind_vertex_array_object(GLuint vao)
{
    glBindVertexArray(vao);
}

void draw_elements(GLenum rendering_mode, GLsizei element_count, std::size_t buffer_offset)
{
    glDrawElements(rendering_mode, element_count, GL_UNSIGNED_SHORT, reinterpret_cast<const void *>(buffer_offset));
}
} // namespace Age::Gfx
