#include "core.h"

#include <GLFW/glfw3.h>
#include <string>

class Window
{
  public:
    Window(int width, int height, const std::string& name);
    ~Window();

    void Update();
    bool ShouldClose();

    GLFWwindow* GetNativeWindow()
    {
        return window;
    }

    int GetWidth()
    {
        return width;
    }

    int GetHeight()
    {
        return height;
    }

  private:
    GLFWwindow* window;
    int width, height;
};