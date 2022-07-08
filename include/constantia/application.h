#pragma once

#include "camera.h"
#include "window.h"
#include "layer.h"

class Application
{
  public:
    Application(int width, int height, const std::string& name);
    ~Application();

    void Run();

    static Application& Get()
    {
        return *instance;
    }
    Window& GetWindow()
    {
        return *window;
    }

    static Camera& GetCamera()
    {
        return *camera;
    }

    static std::vector<Layer*>& GetLayers()
    {
        return layers;
    }

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        camera->MoveCamera(xpos, ypos);
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera->Zoom(xoffset, yoffset);
    }

  private:
    static Application* instance;
    std::unique_ptr<Window> window;
    float delta_time = 0, lastFrame = 0;
    static Camera* camera;

  protected:
    static std::vector<Layer*> layers;
};