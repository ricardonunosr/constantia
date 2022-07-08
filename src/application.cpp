#include "core.h"
#include "application.h"
#include "camera.h"
#include "editor.h"
#include <filesystem>
#include <vector>

Application* Application::instance = nullptr;
Camera* Application::camera = nullptr;
std::vector<Layer*> Application::layers;

Application::Application(int width, int height, const std::string& name)
{
    instance = this;
    window = std::make_unique<Window>(width, height, name);
    camera = new Camera();
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
    while (!window->ShouldClose())
    {
        float current_frame = glfwGetTime();
        delta_time = current_frame - lastFrame;
        lastFrame = current_frame;

        camera->ProcessInput(Application::Get().GetWindow().GetNativeWindow(), delta_time);
        bool editor = camera->GetEditorFlag();
        EditorImGuiRender(editor);
        for (Layer* layer : layers)
        {
            layer->Update(current_frame);
        }
        window->Update();
    }
}
