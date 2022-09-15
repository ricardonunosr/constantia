#include "editor.h"
#include "core.h"

#include "application.h"
#include "examples/framebuffers/framebuffers.h"
#include "examples/sponza/sponza.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static bool examples = true;
static bool showDemo = false;

std::vector<Layer*>& layers = Application::Get().GetLayers();
FrameBuffersLayer* framebuffers = nullptr;
SponzaLayer* sponza = nullptr;

void ImGuiInit(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    framebuffers = new FrameBuffersLayer("FrameBuffers");
    sponza = new SponzaLayer("Sponza");
}

void ImGuiCleanup()
{
    delete framebuffers;
    delete sponza;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorImGuiRender(bool editor, float delta_time)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Dockspace
    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    if (showDemo)
        ImGui::ShowDemoWindow(&showDemo);

    if (editor)
    {
        if (ImGui::Begin("Metrics/Debugger"))
        {
            Metrics& metrics = Application::Get().GetMetrics();

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
                ImGui::MenuItem("Examples", NULL, &examples);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (examples)
        {
            ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
            ImGui::Begin("Scene", &examples);
            if (ImGui::Button("FrameBuffers"))
            {
                layers.clear();
                layers.push_back(framebuffers);
            }
            if (ImGui::Button("Sponza"))
            {
                layers.clear();
                layers.push_back(sponza);
            }
            ImGui::End();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
