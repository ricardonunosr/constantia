#include <glad/gl.h>

#include "application.h"
#include "editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <vector>

Application* Application::s_instance = nullptr;
std::vector<Layer*> Application::layers;
Metrics Application::s_metrics;

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

Application::Application(int width, int height, const std::string& name) : m_width{width}, m_height{height}
{
    s_instance = this;
    s_metrics = {};

    if (glfwInit() == 0)
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (m_window == nullptr)
    {
        glfwTerminate();
        return;
    }
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(m_window);
    // Disable V-Sync
    // glfwSwapInterval(0);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        spdlog::error("Failed to initialize OpenGL context");
        return;
    }

    // Successfully loaded OpenGL
    spdlog::info("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    spdlog::info("Current working directory: {}", std::filesystem::current_path().c_str());
    im_gui_init(m_window);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
}

Application::~Application()
{
    glfwTerminate();
    im_gui_cleanup();
}

void Application::run()
{
    // Main Loop
    while (!(glfwWindowShouldClose(m_window) != 0))
    {
        float current_frame = static_cast<float>(glfwGetTime());
        m_delta_time = current_frame - m_last_frame;
        m_last_frame = current_frame;

        for (Layer* layer : layers)
            layer->update(m_delta_time);

        // UI Scope
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            for (Layer* layer : layers)
                layer->on_ui_render(m_delta_time);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }
}
