#include <functional>
#include <span>
#include <utility>
#include <vector>

#include "Color.hpp"
#include "DDS.hpp"
#include "ECS.hpp"
#include "ErrorHandling.hpp"
#include "MaterialInstances.hpp"
#include "MeshInstances.hpp"
#include "OpenGL.hpp"
#include "Path.hpp"
#include "Rendering.hpp"
#include "ShaderInstances.hpp"
#include "SphericalCamera.hpp"
#include "Transformations.hpp"
#include "UniformBlocks.hpp"

#include "game/InfinitySymbolMesh.hpp"
#include "game/InfinitySymbolScene.hpp"
#include "game/Meshes.hpp"
#include "game/Processing.hpp"
#include "game/ShadersAndMaterials.hpp"

namespace Game
{
using namespace Age;

struct InfinitySymbol
{
    static constexpr Core::ComponentType TYPE{INFINITY_SYMBOL};
};

struct InfinitySymbolShader : public Gfx::Shader
{
    GLint gaussian_texture{-1};
    GLint shininess_texture{-1};
    GLint use_shininess_texture{-1};
    GLuint material_block{GL_INVALID_INDEX};

    InfinitySymbolShader(GLuint shader_program)
        : Shader{shader_program, Gfx::SHADER_LV_MATRIX | Gfx::SHADER_LV_NORMAL_MATRIX | Gfx::SHADER_LIGHT_DATA_BLOCK}
        , gaussian_texture{Gfx::OGL::get_uniform_location(shader_program, "uGaussianTexture")}
        , shininess_texture{Gfx::OGL::get_uniform_location(shader_program, "uShininessTexture")}
        , use_shininess_texture{Gfx::OGL::get_uniform_location(shader_program, "uUseShininessTex")}
        , material_block{Gfx::OGL::get_uniform_block_index(shader_program, "MaterialBlock")}
    {
        Gfx::OGL::bind_uniform_block(shader_program, material_block, MATERIAL_BLOCK_BINDING);
    }
};

struct InfinitySymbolMaterial : public Gfx::Material
{
    int gaussian_texture{};
    int shininess_texture{};
    bool use_shininess_texture{};

    InfinitySymbolMaterial(const Age::Gfx::Shader &shader)
        : Material{shader}
    {
    }

    void apply_properties() const override
    {
        auto &shader = static_cast<const InfinitySymbolShader &>(this->shader);
        Gfx::OGL::set_uniform(shader.gaussian_texture, gaussian_texture);
        Gfx::OGL::set_uniform(shader.shininess_texture, shininess_texture);
        Gfx::OGL::set_uniform(shader.use_shininess_texture, use_shininess_texture);
    }
};

struct MaterialBlock
{
    Math::Vector4 diffuse_color;
    Math::Vector4 specular_color;
    float surface_shininess{1.0f};
    float _padding_[3];
};

void InfinitySymbolScene::init_entities() const
{
    Gfx::MeshId next_mesh_id{Gfx::USER_MESH_START_ID};
    Gfx::ShaderId next_shader_id{0};
    Gfx::MaterialId next_material_id{0};
    Gfx::UniformBufferId next_uniform_buffer_id{0};

    auto no_lighting_color_shader_id = next_shader_id++;
    {
        Gfx::ShaderAsset shader_assets[] = {
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/no_lighting_color.vert"},
            Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/no_lighting.frag"}
        };
        Gfx::create_shader<Gfx::NoLightingColorShader>(no_lighting_color_shader_id, shader_assets);
    }

    constexpr std::size_t GAUSSIAN_TEX_ANGLE_RESOLUTION{512};
    constexpr std::size_t GAUSSIAN_TEX_SHININESS_RESOLUTION{128};

    auto gaussian_terms =
        std::make_unique_for_overwrite<GLubyte[]>(GAUSSIAN_TEX_ANGLE_RESOLUTION * GAUSSIAN_TEX_SHININESS_RESOLUTION);
    for (std::size_t shininess_index{1}; shininess_index <= GAUSSIAN_TEX_SHININESS_RESOLUTION; ++shininess_index)
    {
        float shininess{shininess_index / static_cast<float>(GAUSSIAN_TEX_SHININESS_RESOLUTION)};
        for (std::size_t sqrt_cos_index{}; sqrt_cos_index < GAUSSIAN_TEX_ANGLE_RESOLUTION; ++sqrt_cos_index)
        {
            float sqrt_cos{sqrt_cos_index / static_cast<float>(GAUSSIAN_TEX_ANGLE_RESOLUTION - 1ULL)};
            float cos{sqrt_cos * sqrt_cos};

            float half_angle{std::acos(cos)};
            float gaussian_exponent{half_angle / shininess};
            gaussian_exponent = -(gaussian_exponent * gaussian_exponent);
            float gaussian_term{std::exp(gaussian_exponent)};

            gaussian_terms[(shininess_index - 1) * GAUSSIAN_TEX_ANGLE_RESOLUTION + sqrt_cos_index] =
                static_cast<GLubyte>(gaussian_term * 255.0f);
        }
    }

    GLuint gaussian_texture;
    glGenTextures(1, &gaussian_texture);
    glBindTexture(GL_TEXTURE_2D, gaussian_texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        GAUSSIAN_TEX_ANGLE_RESOLUTION,
        GAUSSIAN_TEX_SHININESS_RESOLUTION,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        gaussian_terms.get()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    Gfx::TextureData texture{};
    if (!Gfx::load_texture_from_dds_file("assets/game/textures/shininess.dds", texture))
        return;

    GLuint shininess_texture;
    glGenTextures(1, &shininess_texture);
    glBindTexture(GL_TEXTURE_2D, shininess_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_R8, texture.width, texture.height, 0, GL_RED, GL_UNSIGNED_BYTE, texture.bytes.get()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    constexpr int gaussian_texture_image_unit{1};
    constexpr int shininess_texture_image_unit{2};

    glActiveTexture(GL_TEXTURE0 + gaussian_texture_image_unit);
    glBindTexture(GL_TEXTURE_2D, gaussian_texture);

    glActiveTexture(GL_TEXTURE0 + shininess_texture_image_unit);
    glBindTexture(GL_TEXTURE_2D, shininess_texture);

    GLuint texture_sampler;
    glGenSamplers(1, &texture_sampler);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(texture_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindSampler(gaussian_texture_image_unit, texture_sampler);
    glBindSampler(shininess_texture_image_unit, texture_sampler);

    // Camera
    Core::EntityId camera_id;
    {
        Gfx::PerspectiveCamera camera{.near_plane_z{0.1f}, .far_plane_z{1000.0f}, .vertical_fov{Math::radians(50.0f)}};

        auto gamma_correction_buffer_id = next_uniform_buffer_id++;
        auto &gamma_correction_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::GammaCorrectionBlock>>(gamma_correction_buffer_id);

        auto projection_buffer_id = next_uniform_buffer_id++;
        auto &projection_buffer =
            Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::ProjectionBlock>>(projection_buffer_id);

        auto lights_buffer_id = next_uniform_buffer_id++;
        auto &lights_buffer = Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<Gfx::LightsBlock>>(lights_buffer_id);

        camera_id = Core::create_entity(
            camera,
            Gfx::WorldToViewMatrix{},
            Gfx::ViewToClipMatrix{
                Math::perspective_proj_matrix(camera.near_plane_z, camera.far_plane_z, 1.0f, camera.vertical_fov)
            },
            Gfx::RenderState{.clear_color{0.75f, 0.75f, 1.0f, 1.0f}},
            Gfx::GammaCorrectionBufferBlockRef{gamma_correction_buffer.get_block()},
            Gfx::ProjectionBufferBlockRef{projection_buffer.get_block()},
            Gfx::LightsBufferBlockRef{lights_buffer.get_block()},
            Input::MouseInput{.motion_sensitivity{0.005f}},
            Gfx::SphericalCamera{
                .origin{0.0f, 0.0f, 0.0f}, .spherical_coord{Math::Vector2{Math::radians(60.0f), 0.0f}, 30.0f}
            },
            GameKeyboardController{}
        );
    }

    // Global settings
    Core::EntityId global_settings_id;
    {
        global_settings_id = Core::create_entity(
            Gfx::GlobalColorSettings{.gamma_inverse{1 / 2.2f}},
            Gfx::GlobalLightSettings{
                .ambient_light_intensity{0.2f, 0.2f, 0.2f, 1.0f},
                .light_attenuation{1.0f / (25.0f * 25.0f)},
                .max_intensity{1.0f}
            }
        );
    }

    // Directional light
    {
        Core::create_entity(
            Core::Transform{.position{Math::normalize(Math::Vector3{1.0f, 1.0f, -1.0f})}},
            Gfx::DirectionalLight{.light_intensity{0.6f, 0.6f, 0.6f, 1.0f}}
        );
    }

    // Point light
    {
        auto material_id = next_material_id++;
        Gfx::create_material<Gfx::NoLightingColorMaterial>(material_id, no_lighting_color_shader_id);

        auto id = Core::create_entity(
            Core::Transform{.position{10.0f, 0.0f, 1.0f}, .scale{0.5f}},
            Gfx::LocalToViewMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::MeshRef{Gfx::CUBE_MESH_ID},
            Gfx::Renderer{},
            Gfx::PointLight{.light_intensity{0.4f, 0.4f, 0.4f, 1.0f}}
        );

        Gfx::init_renderer(id, Gfx::RENDER_WITH_LV_MATRIX);
    }

    // Infinity symbol
    {
        auto infinity_mesh_id = next_mesh_id++;
        Gfx::create_mesh<1>(infinity_mesh_id, std::function{create_infinity_symbol_mesh});

        auto infinity_shader_id = next_shader_id++;
        {
            Gfx::ShaderAsset shader_assets[] = {
                Gfx::ShaderAsset{Gfx::OGL::ShaderType::VERTEX, "shaders/game/infinity_symbol.vert"},
                Gfx::ShaderAsset{Gfx::OGL::ShaderType::FRAGMENT, "shaders/game/infinity_symbol.frag"}
            };
            Gfx::create_shader<InfinitySymbolShader>(infinity_shader_id, shader_assets);
        }

        auto material_id = next_material_id++;
        auto &material = Gfx::create_material<InfinitySymbolMaterial>(material_id, infinity_shader_id);
        material.gaussian_texture = gaussian_texture_image_unit;
        material.shininess_texture = shininess_texture_image_unit;
        material.use_shininess_texture = true;

        auto material_buffer_id = next_uniform_buffer_id++;
        auto &material_buffer = Gfx::create_uniform_buffer<Gfx::ScalarUniformBuffer<MaterialBlock>>(material_buffer_id);
        material_buffer.get_block() = MaterialBlock{
            .diffuse_color{1.0f, 0.673f, 0.043f, 1.0f},
            .specular_color{Math::Vector4{1.0f, 0.673f, 0.043f, 1.0f} * 0.4f},
            .surface_shininess{0.125f}
        };

        auto id = Core::create_entity(
            Core::Transform{.scale{4.0f}},
            Gfx::LocalToViewMatrix{},
            Gfx::LocalToViewNormalMatrix{},
            Gfx::MaterialRef{material_id},
            Gfx::UniformBufferRangeBind{material_buffer.get_block().get_buffer_range(), MATERIAL_BLOCK_BINDING},
            Gfx::MeshRef{infinity_mesh_id},
            Gfx::Renderer{},
            InfinitySymbol{}
        );

        Gfx::init_renderer(
            id, Gfx::RENDER_WITH_LV_MATRIX | Gfx::RENDER_WITH_LV_NORMAL_MATRIX | Gfx::RENDER_WITH_BUFFER_RANGE_BIND
        );
    }
}

void control_infinity_symbol_material(const InfinitySymbol &, const Gfx::MaterialRef &material_ref)
{
    auto &material = static_cast<InfinitySymbolMaterial &>(Gfx::get_material(material_ref.material_id));

    if (Input::is_key_down(GLFW_KEY_T))
        material.use_shininess_texture = true;
    else if (Input::is_key_down(GLFW_KEY_P))
        material.use_shininess_texture = false;
}

void InfinitySymbolScene::run_systems() const
{
    using Core::process_components;

    process_components(control_game_via_keyboard);
    process_components(control_transform_via_keyboard);
    process_components(control_infinity_symbol_material);
    process_components(Gfx::update_spherical_camera_via_input);
    process_components(Gfx::calc_spherical_camera_view_matrix);
}
} // namespace Game
