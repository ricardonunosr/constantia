#include "core.h"

#include "window.h"

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height, const std::string& name) : m_width{width}, m_height{height}
{
    if (glfwInit() == 0)
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (m_window == nullptr)
    {
        glfwTerminate();
        return;
    }
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Make the window's context current */
    glfwMakeContextCurrent(m_window);
    // Disable V-Sync
    glfwSwapInterval(0);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        spdlog::error("Failed to initialize OpenGL context");
        return;
    }

    // Successfully loaded OpenGL
    spdlog::info("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}

Window::~Window()
{
    glfwTerminate();
}

void Window::update()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(m_window) != 0;
}
