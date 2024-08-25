#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <algorithm>
#include <iostream>

#include "Math.hpp"
#include "Matrix.hpp"
#include "Mesh.hpp"
#include "Meshes.hpp"
#include "Shaders.hpp"
#include "Transformations.hpp"
#include "UniformBuffers.hpp"

using namespace Age;
using Math::Matrix4;
using Math::Vector3;

const float mouse_sensitivity{0.005f};

const float z_near{0.1f};
const float z_far{100.0f};
const float min_camera_pitch{Math::radians(0.1f)};
const float max_camera_pitch{Math::radians(179.9f)};
const float vertical_fov{Math::radians(50.0f)};

Matrix4 g_perspective_matrix{};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);

    float aspect_ratio = static_cast<float>(width) / height;
    Math::update_fov(g_perspective_matrix, aspect_ratio, vertical_fov);
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
static double g_mouse_delta_x{};
static double g_mouse_delta_y{};

static bool g_prev_mouse_set{};
static double g_prev_mouse_x{};
static double g_prev_mouse_y{};

void mouse_position_callback(GLFWwindow *window, double x_position, double y_position)
{
    if (!g_prev_mouse_set)
    {
        g_prev_mouse_set = true;
        g_prev_mouse_x = x_position;
        g_prev_mouse_y = y_position;
        return;
    }

    g_mouse_moved = true;
    g_mouse_delta_x = x_position - g_prev_mouse_x;
    g_mouse_delta_y = y_position - g_prev_mouse_y;
    g_prev_mouse_x = x_position;
    g_prev_mouse_y = y_position;
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

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;

    Gfx::VertexLightingShader program{"shaders/basic.vert", "shaders/basic.frag"};
    program.bind_shared_matrices_block(0);

    Gfx::SharedMatricesUniformBuffer shared_matrices{};
    shared_matrices.bind(0);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    float aspect_ratio = static_cast<float>(width) / height;
    g_perspective_matrix = Math::perspective_matrix(z_near, z_far, aspect_ratio, vertical_fov);

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

    const Gfx::Mesh &plane_mesh{Gfx::load_plane_mesh()};
    const Gfx::Mesh &cube_mesh{Gfx::load_cube_mesh()};
    const Gfx::CylinderMesh cylinder_mesh{30};

    float camera_yaw{Math::radians(180.0f)};
    float camera_pitch{Math::radians(90.0f)};

    Vector3 world_up{0.0f, 1.0f, 0.0f};
    Vector3 camera_forward{0.0f, 0.0f, -1.0f};
    Vector3 camera_right{1.0f, 0.0f, 0.0f};
    Vector3 camera_pos{10.0f, 10.0f, 10.0f};
    const float camera_speed{10.0f};

    float cube_x_rotation{};

    float last_frame_time{};
    while (!glfwWindowShouldClose(window))
    {
        float current_time{static_cast<float>(glfwGetTime())};
        float delta_time{current_time - last_frame_time};
        last_frame_time = current_time;

        if (g_mouse_moved)
        {
            g_mouse_moved = false;

            float x_angle{static_cast<float>(g_mouse_delta_x) * mouse_sensitivity};
            float y_angle{static_cast<float>(g_mouse_delta_y) * mouse_sensitivity};

            camera_yaw -= x_angle;
            camera_pitch = std::clamp(camera_pitch + y_angle, min_camera_pitch, max_camera_pitch);

            float sin_pitch{std::sin(camera_pitch)};
            camera_forward.x = std::sin(camera_yaw) * sin_pitch;
            camera_forward.y = std::cos(camera_pitch);
            camera_forward.z = std::cos(camera_yaw) * sin_pitch;

            camera_right = camera_forward.cross(world_up).normalize();
        }

        if (g_horizontal_axis_input != 0)
            camera_pos += camera_right * (g_horizontal_axis_input * camera_speed * delta_time);
        if (g_vertical_axis_input != 0)
            camera_pos += world_up * (g_vertical_axis_input * camera_speed * delta_time);
        if (g_forward_axis_input != 0)
            camera_pos += camera_forward * (g_forward_axis_input * camera_speed * delta_time);

        Matrix4 cam_matrix{camera_matrix(camera_pos, camera_pos + camera_forward, world_up)};

        shared_matrices.set_clip_matrix(g_perspective_matrix);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube_x_rotation += delta_time * 40.0f;
        {
            Gfx::Shader::Use use{program};
            program.set_camera_matrix(cam_matrix);

            {
                Matrix4 world_matrix{Math::translation_matrix(Vector3{0.0f, 0.0f, 0.0f}) *
                                     Math::x_rotation_matrix(Math::radians(-90.0f)) *
                                     Math::scaling_matrix(Vector3{20.0f, 20.0f, 1.0f})};
                program.set_world_matrix(world_matrix);
                plane_mesh.draw();
            }

            {
                Matrix4 world_matrix{Math::translation_matrix(Vector3{5.0f, 2.0f, -10.0f}) *
                                     Math::x_rotation_matrix(Math::radians(cube_x_rotation)) *
                                     Math::scaling_matrix(Vector3{2.0f, 2.0f, 1.0f})};
                program.set_world_matrix(world_matrix);
                cube_mesh.draw();
            }

            {
                Matrix4 world_matrix{Math::translation_matrix(Vector3{0.0f, 2.0f, 0.0f}) *
                                     Math::z_rotation_matrix(Math::radians(90.0f))};
                program.set_world_matrix(world_matrix);
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