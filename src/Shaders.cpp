#include "Shaders.hpp"

namespace Age::Gfx
{
VertexLightingShader::VertexLightingShader(const std::string &vs_path, const std::string &fs_path)
    : Shader{vs_path, fs_path}
    , _model_to_camera_matrix{get_uniform_location("uModelToCameraMatrix")}
    , _shared_matrices_block_index{get_uniform_block_index("SharedMatrices")}
{
}

void VertexLightingShader::set_world_matrix(const Math::Matrix4 &matrix)
{
    _world_matrix = matrix;
    set_uniform(_model_to_camera_matrix, _camera_matrix * _world_matrix);
}

void VertexLightingShader::set_camera_matrix(const Math::Matrix4 &matrix)
{
    _camera_matrix = matrix;
    set_uniform(_model_to_camera_matrix, _camera_matrix * _world_matrix);
}

void VertexLightingShader::bind_shared_matrices_block(GLuint block_binding)
{
    bind_uniform_block(_shared_matrices_block_index, block_binding);
}
} // namespace Age::Gfx