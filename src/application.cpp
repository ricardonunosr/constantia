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

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    // generate texture
    glGenTextures(1, &m_texture_colorbuffer);
    glBindTexture(GL_TEXTURE_2D, m_texture_colorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_colorbuffer, 0);

    // Generate RenderBuffer for depth and/or stencil attachments
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        spdlog::error("Framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float current_frame = glfwGetTime();
        m_delta_time = current_frame - m_last_frame;
        m_last_frame = current_frame;
        spdlog::info("Render frame: {} ms", m_delta_time * 1000.0f);

        s_camera->process_input(Application::get().get_window().get_native_window(), m_delta_time);

        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        for (Layer* layer : layers)
            layer->update(current_frame);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // UI Scope
        {
            bool editor = s_camera->m_editor;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            for (Layer* layer : layers)
                layer->on_ui_render();

            // if (layers.empty())
            editor_im_gui_render(editor);

            // ImGui::ShowDemoWindow();

            if (ImGui::Begin("Main Camera"))
            {
                ImVec2 wsize = ImGui::GetWindowSize();
                ImGui::Image((ImTextureID)m_texture_colorbuffer, wsize, ImVec2(0, 1), ImVec2(1, 0));
                ImGui::End();
            }

            if (ImGui::Begin("Secondary Camera"))
            {
                ImGui::End();
            }

            if (ImGui::Begin("Metrics/Debugger"))
            {
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
