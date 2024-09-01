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
    GLint _normal_model_to_camera_matrix{};
    GLuint _shared_matrices_block_index{};

    GLint _light_position{};
    GLint _light_intensity{};
    GLint _ambient_light_intensity{};

  public:
    DiffuseLightingShader(const std::string &vs_path, const std::string &fs_path);

    void set_camera_matrix(const Math::Matrix4 &matrix);
    void set_normal_camera_matrix(const Math::Matrix3 &matrix);
    void bind_shared_matrices_block(GLuint block_binding);

    void set_light_position(const Math::Vector3 &light_position);
    void set_light_intensity(const Math::Vector4 &light_intensity);
    void set_ambient_light_intensity(const Math::Vector4 &light_intensity);
};
} // namespace Age::Gfx