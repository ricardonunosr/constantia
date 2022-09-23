#pragma once

#include "camera.h"
#include "layer.h"
#include <string>

struct Model;
struct Shader;
struct GLFWwindow;

class SponzaLayer : public Layer
{
  public:
    explicit SponzaLayer(const std::string& name);
    ~SponzaLayer();

    void init() override;
    void de_init() override;
    void update(float delta_time) override;
    void on_ui_render(float delta_time) override;

    static void mouse_callback(GLFWwindow* /*window*/, double xpos, double ypos)
    {
        m_camera->handle_mouse_move(xpos, ypos);
    }

  private:
    std::unique_ptr<Model> m_light;
    std::unique_ptr<Model> m_sponza;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Shader> m_light_shader;

    static std::unique_ptr<Camera> m_camera;
    static std::unique_ptr<Camera> m_second_camera;

    unsigned int m_framebuffer, m_second_framebuffer, m_rbo, m_second_rbo, m_texture_colorbuffer,
        m_texture_second_colorbuffer;
};