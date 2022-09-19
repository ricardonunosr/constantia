#pragma once

#include <GLFW/glfw3.h>
#include <string>

class Window
{
  public:
    Window(int width, int height, const std::string& name);
    ~Window();

    void update();
    bool should_close();

    GLFWwindow* get_native_window()
    {
        return m_window;
    }

    [[nodiscard]] int get_width() const
    {
        return m_width;
    }

    [[nodiscard]] int get_height() const
    {
        return m_height;
    }

  private:
    GLFWwindow* m_window;
    int m_width, m_height;
};