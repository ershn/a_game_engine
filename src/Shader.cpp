#include <fstream>
#include <iostream>
#include <sstream>

#include "Shader.hpp"

namespace Age::Gfx
{
Shader::Use::Use(Shader &program)
    : _program{program}
{
    _program.use_program();
}

Shader::Use::~Use()
{
    _program.use_program(false);
}

Shader::Shader(const std::string &vertex_shader_path, const std::string &fragment_shader_path)
    : _program{create_program(vertex_shader_path, fragment_shader_path)}
{
}

Shader::~Shader()
{
    glDeleteProgram(_program);
}

GLint Shader::get_uniform_location(const std::string &name) const
{
    GLint location{glGetUniformLocation(_program, name.data())};
    if (location == -1)
        std::cerr << "Uniform variable not found: " << name << '\n';
    return location;
}

void Shader::set_uniform(GLint location, float value)
{
    glUniform1f(location, value);
}

void Shader::set_uniform(GLint location, const Math::Matrix4 &matrix)
{
    glUniformMatrix4fv(location, 1, false, static_cast<const GLfloat *>(matrix));
}

GLuint Shader::get_uniform_block_index(const std::string &name) const
{
    GLuint index{glGetUniformBlockIndex(_program, name.data())};
    if (index == GL_INVALID_INDEX)
        std::cerr << "Uniform block not found: " << name << '\n';
    return index;
}

void Shader::bind_uniform_block(GLuint block_index, GLuint block_binding)
{
    glUniformBlockBinding(_program, block_index, block_binding);
}

GLuint Shader::create_program(const std::string &vertex_shader_path,
                              const std::string &fragment_shader_path) const
{
    std::string vertex_shader_source{read_file(vertex_shader_path)};
    std::string fragment_shader_source{read_file(fragment_shader_path)};
    GLuint vertex_shader{create_shader(GL_VERTEX_SHADER, vertex_shader_source)};
    GLuint fragment_shader{create_shader(GL_FRAGMENT_SHADER, fragment_shader_source)};
    GLuint program{create_program(vertex_shader, fragment_shader)};
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

GLuint Shader::create_program(GLuint vertex_shader, GLuint fragment_shader) const
{
    GLuint program = glCreateProgram();
    if (program == 0)
        throw std::runtime_error("Program creation failed");

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    GLint linking_succeeded;
    glGetProgramiv(program, GL_LINK_STATUS, &linking_succeeded);
    if (!linking_succeeded)
    {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        std::string log{static_cast<std::string::size_type>(log_length - 1), '\0',
                        std::string::allocator_type{}};
        glGetProgramInfoLog(program, log_length, nullptr, log.data());
        std::cerr << "Program linking failed:\n" << log;

        throw std::runtime_error("Program linking failed");
    }

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    return program;
}

GLuint Shader::create_shader(GLenum shader_type, const std::string &source_code) const
{
    GLuint shader = glCreateShader(shader_type);
    if (shader == 0)
        throw std::runtime_error("Shader creation failed");

    const char *source{source_code.data()};
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compilation_succeeded;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_succeeded);
    if (!compilation_succeeded)
    {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        std::string log{static_cast<std::string::size_type>(log_length - 1), '\0',
                        std::string::allocator_type{}};
        glGetShaderInfoLog(shader, log_length, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log;

        throw std::runtime_error("Shader compilation failed");
    }

    return shader;
}

void Shader::use_program(bool use)
{
    glUseProgram(use ? _program : 0);
}

std::string Shader::read_file(const std::string &path) const
{
    std::ifstream file{path};
    file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    std::stringstream stringstream{};
    stringstream << file.rdbuf();
    return std::string{std::move(stringstream).str()};
}
} // namespace Age::Gfx