#include "core.h"

#include "application.h"
#include "camera.h"
#include "editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>
#include <vector>

Application* Application::s_instance = nullptr;
std::unique_ptr<Camera> Application::s_camera = nullptr;
std::vector<Layer*> Application::layers;
Metrics Application::s_metrics;

Application::Application(int width, int height, const std::string& name)
{
    s_instance = this;
    m_window = std::make_unique<Window>(width, height, name);
    s_metrics = {};
    s_camera = std::make_unique<Camera>();
    glfwSetScrollCallback(m_window->get_native_window(), scroll_callback);
    glfwSetCursorPosCallback(m_window->get_native_window(), mouse_callback);

    im_gui_init(m_window->get_native_window());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
    spdlog::info("Current working directory: {}", std::filesystem::current_path().c_str());
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
        float current_frame = glfwGetTime();
        m_delta_time = current_frame - m_last_frame;
        m_last_frame = current_frame;

        s_camera->process_input(Application::get().get_window().get_native_window(), m_delta_time);
        for (Layer* layer : layers)
            layer->update(current_frame);

        // UI Scope
        {
            bool editor = s_camera->get_editor_flag();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            for (Layer* layer : layers)
                layer->on_ui_render();

            if (layers.empty())
                editor_im_gui_render(editor);

            ImGui::ShowDemoWindow();

            if (ImGui::Begin("Metrics/Debugger"))
            {
                spdlog::info("glfwGetTime Ms: {}", m_delta_time * 1000.0f);
                ImGui::Text("Application average %.3f ms/frame (%.3f FPS)", m_delta_time * 1000.0f,
                            1000.0f / (1000.0f * m_delta_time));
                ImGui::Text("%d vertices, %d indices (%d triangles)", s_metrics.vertex_count, s_metrics.indices_count,
                            s_metrics.indices_count / 3);
                ImGui::Separator();

                ImGui::End();
            }
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Editor"))
                {
                    if (ImGui::Button("Go Back"))
                    {
                        layers.clear();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        m_window->update();
    }
}
