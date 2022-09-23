#include "core.h"

#include "application.h"
#include "editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>
#include <vector>

Application* Application::s_instance = nullptr;
std::vector<Layer*> Application::layers;
Metrics Application::s_metrics;

Application::Application(int width, int height, const std::string& name)
{
    s_instance = this;
    m_window = std::make_unique<Window>(width, height, name);
    s_metrics = {};

    spdlog::info("Current working directory: {}", std::filesystem::current_path().c_str());
    im_gui_init(m_window->get_native_window());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
}

Application::~Application()
{
    im_gui_cleanup();
}

void Application::run()
{
    // Main Loop
    while (!m_window->should_close())
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

            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            for (Layer* layer : layers)
                layer->on_ui_render(m_delta_time);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        m_window->update();
    }
}
