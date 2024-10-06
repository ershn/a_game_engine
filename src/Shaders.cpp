#include "Shaders.hpp"

namespace Age::Gfx
{
NoLightingShader::NoLightingShader(const std::string &vs_path, const std::string &fs_path)
    : Shader{vs_path, fs_path}
    , _model_to_camera_matrix{get_uniform_location("uModelToCameraMatrix")}
    , _shared_matrices_block_index{get_uniform_block_index("SharedMatrices")}
{
}

void NoLightingShader::set_camera_matrix(const Math::Matrix4 &matrix)
{
    set_uniform(_model_to_camera_matrix, matrix);
}

void NoLightingShader::bind_shared_matrices_block(GLuint block_binding)
{
    bind_uniform_block(_shared_matrices_block_index, block_binding);
}

NoLightingColorShader::NoLightingColorShader(const std::string &vs_path, const std::string &fs_path)
    : NoLightingShader{vs_path, fs_path}
    , _color{get_uniform_location("uColor")}
{
}

void NoLightingColorShader::set_color(const Math::Vector4 &color)
{
    set_uniform(_color, color);
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
    , _model_to_camera_normal_matrix{get_uniform_location("uModelToCameraNormalMatrix")}
    , _shared_matrices_block_index{get_uniform_block_index("SharedMatrices")}
    , _fragment_position_data_block_index{get_uniform_block_index("FragmentPositionData")}
    , _camera_light_position{get_uniform_location("uCameraLightPosition")}
    , _light_intensity{get_uniform_location("uLightIntensity")}
    , _light_attenuation{get_uniform_location("uLightAttenuation")}
    , _ambient_light_intensity{get_uniform_location("uAmbientLightIntensity")}
    , _specular_color{get_uniform_location("uSpecularColor")}
    , _surface_shininess{get_uniform_location("uSurfaceShininess")}
{
}

void FragmentLightingShader::set_camera_matrix(const Math::Matrix4 &matrix)
{
    set_uniform(_model_to_camera_matrix, matrix);
}

void FragmentLightingShader::set_camera_normal_matrix(const Math::Matrix3 &matrix)
{
    set_uniform(_model_to_camera_normal_matrix, matrix);
}

void FragmentLightingShader::bind_shared_matrices_block(GLuint block_binding)
{
    bind_uniform_block(_shared_matrices_block_index, block_binding);
}

void FragmentLightingShader::bind_fragment_position_data_block(GLuint block_binding)
{
    bind_uniform_block(_fragment_position_data_block_index, block_binding);
}

void FragmentLightingShader::set_camera_light_position(const Math::Vector3 &light_position)
{
    set_uniform(_camera_light_position, light_position);
}

void FragmentLightingShader::set_light_intensity(const Math::Vector4 &light_intensity)
{
    set_uniform(_light_intensity, light_intensity);
}

void FragmentLightingShader::set_light_attenuation(float light_attenuation)
{
    set_uniform(_light_attenuation, light_attenuation);
}

void FragmentLightingShader::set_ambient_light_intensity(const Math::Vector4 &light_intensity)
{
    set_uniform(_ambient_light_intensity, light_intensity);
}

void FragmentLightingShader::set_specular_color(const Math::Vector4 &specular_color)
{
    set_uniform(_specular_color, specular_color);
}

void FragmentLightingShader::set_surface_shininess(float surface_shininess)
{
    set_uniform(_surface_shininess, surface_shininess);
}

FragmentLightingColorShader::FragmentLightingColorShader(const std::string &vs_path,
                                                         const std::string &fs_path)
    : FragmentLightingShader{vs_path, fs_path}
    , _color{get_uniform_location("uColor")}
{
}

void FragmentLightingColorShader::set_color(const Math::Vector4 &color)
{
    set_uniform(_color, color);
}
} // namespace Age::Gfx
