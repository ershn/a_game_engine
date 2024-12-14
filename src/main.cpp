#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <algorithm>
#include <iostream>
#include <limits>

#include "Components.hpp"
#include "ECS.hpp"
#include "FlyCamera.hpp"
#include "Math.hpp"
#include "Matrix.hpp"
#include "Mesh.hpp"
#include "Meshes.hpp"
#include "Quaternion.hpp"
#include "Rendering.hpp"
#include "Shaders.hpp"
#include "SphericalCamera.hpp"
#include "Transformations.hpp"
#include "UniformBuffers.hpp"

using namespace Age;
using Math::Matrix3;
using Math::Matrix4;
using Math::Quaternion;
using Math::Vector2;
using Math::Vector3;
using Math::Vector4;

const float mouse_sensitivity{0.005f};

const float z_near{0.1f};
const float z_far{100.0f};
const float vertical_fov{Math::radians(50.0f)};

int g_viewport_width{};
int g_viewport_height{};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    g_viewport_width = width;
    g_viewport_height = height;

    glViewport(0, 0, width, height);
}

int g_horizontal_axis_input{};
int g_vertical_axis_input{};
int g_forward_axis_input{};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    int press_change{};
    if (action == GLFW_PRESS)
        press_change = 1;
    else if (action == GLFW_RELEASE)
        press_change = -1;

    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_KEY_W:
        g_forward_axis_input += press_change;
        break;
    case GLFW_KEY_S:
        g_forward_axis_input -= press_change;
        break;
    case GLFW_KEY_D:
        g_horizontal_axis_input += press_change;
        break;
    case GLFW_KEY_A:
        g_horizontal_axis_input -= press_change;
        break;
    case GLFW_KEY_E:
        g_vertical_axis_input += press_change;
        break;
    case GLFW_KEY_Q:
        g_vertical_axis_input -= press_change;
        break;
    }
}

static bool g_mouse_moved{};
static double g_mouse_position_delta_x{};
static double g_mouse_position_delta_y{};

void mouse_position_callback(GLFWwindow *window, double x_position, double y_position)
{
    static bool s_prev_mouse_set{};
    static double s_prev_mouse_x{};
    static double s_prev_mouse_y{};

    if (!s_prev_mouse_set)
    {
        s_prev_mouse_set = true;
        s_prev_mouse_x = x_position;
        s_prev_mouse_y = y_position;
        return;
    }

    g_mouse_moved = true;
    g_mouse_position_delta_x = x_position - s_prev_mouse_x;
    g_mouse_position_delta_y = y_position - s_prev_mouse_y;
    s_prev_mouse_x = x_position;
    s_prev_mouse_y = y_position;
}

static bool g_mouse_scrolled{};
static double g_mouse_scroll_delta_x{};
static double g_mouse_scroll_delta_y{};

void mouse_scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
    g_mouse_scrolled = true;
    g_mouse_scroll_delta_x = x_offset;
    g_mouse_scroll_delta_y = y_offset;
}

void error_callback(int error, const char *description)
{
    std::cerr << "Error: " << description << std::endl;
}

class GlfwInitializer final
{
  public:
    using error_callback_t = void (*)(int error, const char *description);

    GlfwInitializer(error_callback_t error_callback)
    {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit())
            throw std::runtime_error("GLFW initialization failed");
    }

    ~GlfwInitializer()
    {
        glfwTerminate();

        glfwSetErrorCallback(nullptr);
    }
};

void run()
{
    ECS::init_ecs();

    GlfwInitializer glfw_initializer{error_callback};

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Age", nullptr, nullptr);
    if (window == nullptr)
        throw std::runtime_error("Window/context creation failed");

    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
        throw std::runtime_error("OpenGL loading failed");

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    Gfx::NoLightingShader no_lighting_shader{"shaders/no_lighting.vert", "shaders/no_lighting.frag"};
    no_lighting_shader.bind_shared_matrices_block(0);

    Gfx::NoLightingColorShader no_lighting_color_shader{"shaders/no_lighting_color.vert", "shaders/no_lighting.frag"};
    no_lighting_color_shader.bind_shared_matrices_block(0);

    Gfx::DiffuseLightingShader diffuse_lighting_shader{"shaders/diffuse_lighting.vert", "shaders/no_lighting.frag"};
    diffuse_lighting_shader.bind_shared_matrices_block(0);

    Gfx::FragmentLightingShader fragment_lighting_shader{"shaders/fragment_lighting.vert",
                                                         "shaders/fragment_lighting.frag"};
    fragment_lighting_shader.bind_shared_matrices_block(0);
    fragment_lighting_shader.bind_fragment_position_data_block(1);

    // Gfx::FragmentLightingColorShader fragment_lighting_color_shader{
    //     "shaders/fragment_lighting_color.vert", "shaders/fragment_lighting.frag"};
    // fragment_lighting_color_shader.bind_shared_matrices_block(0);
    // fragment_lighting_color_shader.bind_fragment_position_data_block(1);

    Gfx::SharedMatricesUniformBuffer shared_matrices_buffer{};
    shared_matrices_buffer.bind(0);

    Gfx::FragmentPositionDataUniformBuffer fragment_position_data_buffer{};
    fragment_position_data_buffer.bind(1);

    glfwGetFramebufferSize(window, &g_viewport_width, &g_viewport_height);
    glViewport(0, 0, g_viewport_width, g_viewport_height);

    float aspect_ratio{static_cast<float>(g_viewport_width) / g_viewport_height};
    Matrix4 perspective_matrix{Math::perspective_matrix(z_near, z_far, aspect_ratio, vertical_fov)};

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0, 1.0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSwapInterval(1);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_position_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    const Gfx::Mesh &plane_mesh{Gfx::load_plane_mesh()};
    const Gfx::Mesh &cube_mesh{Gfx::load_cube_mesh()};
    const Gfx::CylinderMesh cylinder_mesh{Vector3{1.0f, 0.0f, 0.0f}, 30};

    Vector4 directional_light{Math::normalize(Vector4{0.866f, 0.5f, 0.0f, 0.0f})};

    Vector4 point_light_world_position{3.0f, 1.0f, 0.0f, 1.0f};

    Vector4 light_intensity{0.8f, 0.8f, 0.8f, 1.0f};
    float light_attenuation{0.2f};
    Vector4 ambient_light_intensity{0.2f, 0.2f, 0.2f, 1.0f};

    Vector3 world_up{0.0f, 1.0f, 0.0f};
    // Gfx::FlyCamera fly_camera{{0.0f, 2.0f, 2.0f}, {Math::radians(90.0f), 0.0f}, world_up};
    // const float camera_speed{10.0f};
    Gfx::SphericalCamera spherical_camera{{0.0f, 2.0f, 0.0f}, {Math::radians(60.0f), 0.0f}, 5.0f, world_up};

    Quaternion cylinder_world_rotation{Math::axis_angle_quaternion(Vector3{0.0f, 0.0f, 1.0f}, Math::radians(-20.0f))};
    float cylinder_shininess{0.5f};

    float last_frame_time{};
    while (!glfwWindowShouldClose(window))
    {
        float aspect_ratio{static_cast<float>(g_viewport_width) / g_viewport_height};
        Math::update_fov(perspective_matrix, aspect_ratio, vertical_fov);

        shared_matrices_buffer.set_clip_matrix(perspective_matrix);
        fragment_position_data_buffer.set_viewport_dimensions(g_viewport_width, g_viewport_height);
        fragment_position_data_buffer.set_inverse_clip_matrix(perspective_matrix.inverted());

        float current_time{static_cast<float>(glfwGetTime())};
        float delta_time{current_time - last_frame_time};
        last_frame_time = current_time;

        if (g_mouse_moved)
        {
            g_mouse_moved = false;

            Vector2 camera_angles{static_cast<float>(-g_mouse_position_delta_y),
                                  static_cast<float>(g_mouse_position_delta_x)};
            spherical_camera.add_angles(camera_angles * mouse_sensitivity);
        }
        if (g_mouse_scrolled)
        {
            g_mouse_scrolled = false;

            spherical_camera.add_distance(static_cast<float>(g_mouse_scroll_delta_y) * -0.5f);
        }

        // Vector3 axial_movement{static_cast<float>(g_horizontal_axis_input),
        //                        static_cast<float>(g_vertical_axis_input),
        //                        static_cast<float>(-g_forward_axis_input)};
        // fly_camera.add_axial_movement(axial_movement * camera_speed * delta_time);

        Matrix4 cam_matrix{spherical_camera.calc_camera_matrix()};

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
            cylinder_shininess = std::clamp(cylinder_shininess + delta_time, 0.01f, 1.0f);
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
            cylinder_shininess = std::clamp(cylinder_shininess - delta_time, 0.01f, 1.0f);

        glClearColor(0.294f, 0.22f, 0.192f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            Gfx::Shader::Use use{no_lighting_color_shader};

            Matrix4 world_matrix{Math::translation_matrix(Math::xyz(point_light_world_position)) *
                                 Matrix4{Math::scaling_matrix(Vector3{0.3f})}};
            no_lighting_color_shader.set_camera_matrix(cam_matrix * world_matrix);

            no_lighting_color_shader.set_color(Vector4{1.0f});

            cube_mesh.draw();
        }

        {
            Gfx::Shader::Use use{fragment_lighting_shader};

            fragment_lighting_shader.set_camera_light_position(Math::xyz(cam_matrix * point_light_world_position));
            fragment_lighting_shader.set_light_intensity(light_intensity);
            fragment_lighting_shader.set_light_attenuation(light_attenuation);
            fragment_lighting_shader.set_ambient_light_intensity(ambient_light_intensity);
            fragment_lighting_shader.set_specular_color(Vector4{1.0f, 1.0f, 1.0f, 1.0f});
            fragment_lighting_shader.set_surface_shininess(0.8f);

            {
                Matrix4 world_matrix{Math::translation_matrix(Vector3{0.0f, 0.0f, 0.0f}) *
                                     Matrix4{Math::x_rotation_matrix(Math::radians(-90.0f))} *
                                     Matrix4{Math::scaling_matrix(Vector3{100.0f, 100.0f, 1.0f})}};

                Matrix4 camera_matrix{cam_matrix * world_matrix};
                fragment_lighting_shader.set_camera_matrix(camera_matrix);
                fragment_lighting_shader.set_camera_normal_matrix(Matrix3{camera_matrix}.invert().transpose());

                plane_mesh.draw();
            }

            {
                Matrix4 world_matrix{Math::translation_matrix(Vector3{5.0f, 2.0f, -10.0f}) *
                                     Matrix4{Math::x_rotation_matrix(current_time)} *
                                     Matrix4{Math::scaling_matrix(Vector3{3.0f, 3.0f, 2.0f})}};

                Matrix4 camera_matrix{cam_matrix * world_matrix};
                fragment_lighting_shader.set_camera_matrix(camera_matrix);
                fragment_lighting_shader.set_camera_normal_matrix(Matrix3{camera_matrix}.invert().transpose());

                cube_mesh.draw();
            }

            {
                int rotation_direction{};
                if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
                    rotation_direction += 1;
                if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
                    rotation_direction -= 1;

                if (rotation_direction != 0)
                {
                    float angle{Math::radians(rotation_direction * 45.0f * delta_time)};
                    Quaternion rotation{Math::axis_angle_quaternion({1.0f, 0.0f, 0.0f}, angle)};
                    cylinder_world_rotation = Math::normalize(rotation * cylinder_world_rotation);
                }

                Matrix4 world_matrix{Math::translation_matrix(Vector3{0.0f, 2.0f, 0.0f}) *
                                     Matrix4{Math::rotation_matrix(cylinder_world_rotation)} *
                                     Matrix4{Math::scaling_matrix(Vector3{1.0f, 1.0f, 0.8f})}};

                Matrix4 camera_matrix{cam_matrix * world_matrix};
                fragment_lighting_shader.set_camera_matrix(camera_matrix);
                fragment_lighting_shader.set_camera_normal_matrix(Matrix3{camera_matrix}.invert().transpose());

                if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
                    fragment_lighting_shader.set_specular_color(Vector4{1.0f, 0.0f, 0.0f, 1.0f});
                fragment_lighting_shader.set_surface_shininess(cylinder_shininess);

                cylinder_mesh.draw();
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
}

int main()
{
#ifdef _DEBUG
    run();
    return 0;
#else
    try
    {
        run();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
#endif
}
