#include "core.h"

#include "application.h"
#include "camera.h"
#include "editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>
#include <vector>

Application* Application::instance = nullptr;
std::unique_ptr<Camera> Application::camera = nullptr;
std::vector<Layer*> Application::layers;
Metrics Application::metrics;

Application::Application(int width, int height, const std::string& name)
{
    instance = this;
    window = std::make_unique<Window>(width, height, name);
    metrics = {};
    camera = std::make_unique<Camera>();
    glfwSetScrollCallback(window->GetNativeWindow(), scroll_callback);
    glfwSetCursorPosCallback(window->GetNativeWindow(), mouse_callback);

    ImGuiInit(window->GetNativeWindow());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
    spdlog::info("Current working directory: {}", std::filesystem::current_path().c_str());
}

Application::~Application()
{
    ImGuiCleanup();
}

void Application::Run()
{
    // Main Loop
    while (!window->ShouldClose())
    {
        float current_frame = glfwGetTime();
        delta_time = current_frame - lastFrame;
        lastFrame = current_frame;

        camera->ProcessInput(Application::Get().GetWindow().GetNativeWindow(), delta_time);
        for (Layer* layer : layers)
            layer->Update(current_frame);

        // UI Scope
        {
            bool editor = camera->GetEditorFlag();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            for (Layer* layer : layers)
                layer->OnUIRender();

            if (layers.empty())
                EditorImGuiRender(editor, delta_time);

            ImGui::ShowDemoWindow();

            if (ImGui::Begin("Metrics/Debugger"))
            {
                spdlog::info("glfwGetTime Ms: {}", delta_time * 1000.0f);
                ImGui::Text("Application average %.3f ms/frame (%.3f FPS)", delta_time * 1000.0f,
                            1000.0f / (1000.0f * delta_time));
                ImGui::Text("%d vertices, %d indices (%d triangles)", metrics.vertexCount, metrics.indicesCount,
                            metrics.indicesCount / 3);
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

        window->Update();
    }
}
