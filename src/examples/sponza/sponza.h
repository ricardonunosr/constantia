#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "camera.h"
#include "layer.h"
#include "opengl_renderer.h"
#include <memory>
#include <string>

struct Model;
struct Shader;

struct SponzaShader
{
    OpenGLProgramCommon common;

    GLint light_position;
    GLint light_ambient;
    GLint light_diffuse;
    GLint light_specular;
    GLint light_constant;
    GLint light_linear;
    GLint light_quadratic;
};

class SponzaLayer : public Layer
{
  public:
    explicit SponzaLayer(const std::string& name);

    void update(float delta_time) const override;
    void on_ui_render(float delta_time) const override;

    static void mouse_callback(GLFWwindow* /*window*/, double xpos, double ypos)
    {
        m_camera->handle_mouse_move(xpos, ypos);
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            m_camera->m_enabled = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_camera->m_enabled = false;
        }
    }

  private:
    std::unique_ptr<Model> m_light;
    std::unique_ptr<Model> m_sponza;
    SponzaShader* m_shader;
    OpenGLProgramCommon* m_light_shader;

    static std::unique_ptr<Camera> m_camera;
    static std::unique_ptr<Camera> m_second_camera;

    unsigned int m_framebuffer, m_rbo, m_texture_colorbuffer;
};