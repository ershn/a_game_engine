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
    , _light_position{get_uniform_location("uLightPosition")}
    , _light_intensity{get_uniform_location("uLightIntensity")}
    , _ambient_light_intensity{get_uniform_location("uAmbientLightIntensity")}
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

void DiffuseLightingShader::set_light_position(const Math::Vector3 &light_position)
{
    set_uniform(_light_position, light_position);
}

void DiffuseLightingShader::set_light_intensity(const Math::Vector4 &light_intensity)
{
    set_uniform(_light_intensity, light_intensity);
}

void DiffuseLightingShader::set_ambient_light_intensity(const Math::Vector4 &light_intensity)
{
    set_uniform(_ambient_light_intensity, light_intensity);
}

FragmentLightingShader::FragmentLightingShader(const std::string &vs_path,
                                               const std::string &fs_path)
    : Shader{vs_path, fs_path}
    , _model_to_camera_matrix{get_uniform_location("uModelToCameraMatrix")}
    , _shared_matrices_block_index{get_uniform_block_index("SharedMatrices")}
    , _model_light_position{get_uniform_location("uModelLightPosition")}
    , _light_intensity{get_uniform_location("uLightIntensity")}
    , _ambient_light_intensity{get_uniform_location("uAmbientLightIntensity")}
{
}

void FragmentLightingShader::set_camera_matrix(const Math::Matrix4 &matrix)
{
    set_uniform(_model_to_camera_matrix, matrix);
}

void FragmentLightingShader::bind_shared_matrices_block(GLuint block_binding)
{
    bind_uniform_block(_shared_matrices_block_index, block_binding);
}

void FragmentLightingShader::set_model_light_position(const Math::Vector3 &light_position)
{
    set_uniform(_model_light_position, light_position);
}

void FragmentLightingShader::set_light_intensity(const Math::Vector4 &light_intensity)
{
    set_uniform(_light_intensity, light_intensity);
}

void FragmentLightingShader::set_ambient_light_intensity(const Math::Vector4 &light_intensity)
{
    set_uniform(_ambient_light_intensity, light_intensity);
}
} // namespace Age::Gfx
