#include <fstream>
#include <iostream>
#include <sstream>

#include "Shader.hpp"

namespace Age::Gfx
{
namespace
{
std::string read_file(std::string_view path)
{
    std::ifstream file{path.data()};
    file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    std::stringstream stringstream{};
    stringstream << file.rdbuf();
    return std::string{std::move(stringstream).str()};
}

GLuint create_shader(GLenum shader_type, std::string_view source_code)
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

        std::string log{static_cast<std::string::size_type>(log_length - 1), '\0', std::string::allocator_type{}};
        glGetShaderInfoLog(shader, log_length, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log;

        throw std::runtime_error("Shader compilation failed");
    }

    return shader;
}

GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader)
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

        std::string log{static_cast<std::string::size_type>(log_length - 1), '\0', std::string::allocator_type{}};
        glGetProgramInfoLog(program, log_length, nullptr, log.data());
        std::cerr << "Program linking failed:\n" << log;

        throw std::runtime_error("Program linking failed");
    }

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    return program;
}
} // namespace

Shader::Shader(GLuint shader_program, unsigned int options)
    : shader_program{shader_program}
    , local_to_view_matrix{OGL::get_uniform_location(shader_program, "uLocalToViewMatrix")}
    , gamma_correction_block{OGL::get_uniform_block_index(shader_program, "GammaCorrectionBlock")}
    , projection_block{OGL::get_uniform_block_index(shader_program, "ProjectionBlock")}
{
    if (options & SHADER_VIEW_NORMAL_MATRIX)
        local_to_view_normal_matrix = OGL::get_uniform_location(shader_program, "uLocalToViewNormalMatrix");
    if (options & SHADER_LIGHT_DATA_BLOCK)
        light_data_block = OGL::get_uniform_block_index(shader_program, "LightsBlock");
}

std::vector<std::unique_ptr<Shader>> g_shaders{};

void init_shader_system()
{
    g_shaders.reserve(256);
}

GLuint create_shader_program(std::string_view vertex_shader_path, std::string_view fragment_shader_path)
{
    std::string vertex_shader_source{read_file(vertex_shader_path)};
    std::string fragment_shader_source{read_file(fragment_shader_path)};
    GLuint vertex_shader{create_shader(GL_VERTEX_SHADER, vertex_shader_source)};
    GLuint fragment_shader{create_shader(GL_FRAGMENT_SHADER, fragment_shader_source)};
    GLuint program{create_shader_program(vertex_shader, fragment_shader)};
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

const Shader &get_shader(ShaderId shader_id)
{
    return *g_shaders[shader_id];
}
} // namespace Age::Gfx
