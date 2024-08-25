#pragma once

#include "Matrix.hpp"
#include "Shader.hpp"

namespace Age::Gfx
{
class VertexLightingShader : public Shader
{
    GLint _model_to_camera_matrix{};

    Math::Matrix4 _world_matrix{1.0f};
    Math::Matrix4 _camera_matrix{1.0f};

    GLuint _shared_matrices_block_index{};

  public:
    VertexLightingShader(const std::string &vs_path, const std::string &fs_path);

    void set_world_matrix(const Math::Matrix4 &matrix);
    void set_camera_matrix(const Math::Matrix4 &matrix);

    void bind_shared_matrices_block(GLuint block_binding);
};
} // namespace Age::Gfx