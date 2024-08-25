#pragma once

#include "Matrix.hpp"
#include "Shader.hpp"

namespace Age::Gfx
{
class BasicShader : public Shader
{
    GLint _model_to_camera_matrix{};

    GLuint _shared_matrices_block_index{};

  public:
    BasicShader(const std::string &vs_path, const std::string &fs_path);

    void set_camera_matrix(const Math::Matrix4 &matrix);

    void bind_shared_matrices_block(GLuint block_binding);
};

class DiffuseLightingShader : public Shader
{
    GLint _model_to_camera_matrix{};
    GLuint _shared_matrices_block_index{};

    GLint _direction_to_light{};
    GLint _light_intensity{};

  public:
    DiffuseLightingShader(const std::string &vs_path, const std::string &fs_path);

    void set_camera_matrix(const Math::Matrix4 &matrix);
    void bind_shared_matrices_block(GLuint block_binding);

    void set_direction_to_light(const Math::Vector3 &direction_to_light);
    void set_light_intensity(const Math::Vector4 &light_intensity);
};
} // namespace Age::Gfx