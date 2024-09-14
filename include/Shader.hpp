#pragma once

#include <glad/gl.h>

#include <string>

#include "Matrix.hpp"

namespace Age::Gfx
{
class Shader
{
    GLuint _program{};

  public:
    class Use final
    {
        Shader &_program;

      public:
        Use(Shader &program);
        ~Use();
    };

    Shader(const std::string &vertex_shader_path, const std::string &fragment_shader_path);
    virtual ~Shader();

  protected:
    GLint get_uniform_location(const std::string &name) const;

    void set_uniform(GLint location, float value);
    void set_uniform(GLint location, const Math::Vector3 &vector);
    void set_uniform(GLint location, const Math::Vector4 &vector);
    void set_uniform(GLint location, const Math::Matrix3 &matrix);
    void set_uniform(GLint location, const Math::Matrix4 &matrix);

    GLuint get_uniform_block_index(const std::string &name) const;

    void bind_uniform_block(GLuint block_index, GLuint block_binding);

  private:
    GLuint create_program(const std::string &vertex_shader_path,
                          const std::string &fragment_shader_path) const;

    GLuint create_program(GLuint vertex_shader, GLuint fragment_shader) const;

    GLuint create_shader(GLenum shader_type, const std::string &source_code) const;

    void use_program(bool use = true);

    std::string read_file(const std::string &path) const;
};
} // namespace Age::Gfx
