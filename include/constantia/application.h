#pragma once

#include "camera.h"
#include "layer.h"
#include "window.h"

struct Metrics
{
    uint32_t vertex_count;
    uint32_t indices_count;
};

class Application
{
  public:
    Application(int width, int height, const std::string& name);
    ~Application();

    void run();

    static Application& get()
    {
        return *s_instance;
    }
    Window& get_window()
    {
        return *m_window;
    }

    static Camera& get_camera()
    {
        return *s_camera;
    }

    static std::vector<Layer*>& get_layers()
    {
        return layers;
    }

    static Metrics& get_metrics()
    {
        return s_metrics;
    }

    static void mouse_callback(GLFWwindow* /*window*/, double xpos, double ypos)
    {
        s_camera->move_camera(xpos, ypos);
    }

    static void scroll_callback(GLFWwindow* /*window*/, double xoffset, double yoffset)
    {
        s_camera->zoom(xoffset, yoffset);
    }

  private:
    static Application* s_instance;
    std::unique_ptr<Window> m_window;
    float m_delta_time = 0, m_last_frame = 0;
    static std::unique_ptr<Camera> s_camera;
    static Metrics s_metrics;

  protected:
    static std::vector<Layer*> layers;
};