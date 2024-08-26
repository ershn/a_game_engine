#include "Shaders.hpp"

namespace Age::Gfx
{
BasicShader::BasicShader(const std::string &vs_path, const std::string &fs_path)
    : Shader{vs_path, fs_path}
    , _model_to_camera_matrix{get_uniform_location("uModelToCameraMatrix")}
    , _shared_matrices_block_index{get_uniform_block_index("SharedMatrices")}
{
}

void BasicShader::set_camera_matrix(const Math::Matrix4 &matrix)
{
    set_uniform(_model_to_camera_matrix, matrix);
}

void BasicShader::bind_shared_matrices_block(GLuint block_binding)
{
    bind_uniform_block(_shared_matrices_block_index, block_binding);
}

DiffuseLightingShader::DiffuseLightingShader(const std::string &vs_path, const std::string &fs_path)
    : Shader{vs_path, fs_path}
    , _model_to_camera_matrix{get_uniform_location("uModelToCameraMatrix")}
    , _normal_model_to_camera_matrix{get_uniform_location("uNormalModelToCameraMatrix")}
    , _shared_matrices_block_index{get_uniform_block_index("SharedMatrices")}
    , _direction_to_light{get_uniform_location("uDirectionToLight")}
    , _light_intensity{get_uniform_location("uLightIntensity")}
{
}

void DiffuseLightingShader::set_camera_matrix(const Math::Matrix4 &matrix)
{
    set_uniform(_model_to_camera_matrix, matrix);
}

void DiffuseLightingShader::set_normal_camera_matrix(const Math::Matrix3 &matrix)
{
    set_uniform(_normal_model_to_camera_matrix, matrix);
}

void DiffuseLightingShader::bind_shared_matrices_block(GLuint block_binding)
{
    bind_uniform_block(_shared_matrices_block_index, block_binding);
}

void DiffuseLightingShader::set_direction_to_light(const Math::Vector3 &direction_to_light)
{
    set_uniform(_direction_to_light, direction_to_light);
}

void DiffuseLightingShader::set_light_intensity(const Math::Vector4 &light_intensity)
{
    set_uniform(_light_intensity, light_intensity);
}
} // namespace Age::Gfx