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

struct Metrics
{
    uint32_t vertexCount;
};

void ImGuiInit(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}

void ImGuiCleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorImGuiRender(bool editor, float delta_time)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showDemo)
        ImGui::ShowDemoWindow(&showDemo);

    if (editor)
    {
        if (ImGui::Begin("Dear ImGui Metrics/Debugger"))
        {

            // Basic info
            ImGui::Text("Dear ImGui %s", ImGui::GetVersion());

            ImGui::Text("Application average %.3f ms/frame ( FPS)", delta_time * 1000.0f);
            // ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices,
            //             io.MetricsRenderIndices / 3);
            // ImGui::Text("%d visible windows, %d active allocations", io.MetricsRenderWindows,
            // io.MetricsActiveAllocations);

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
                std::vector<Layer*>& layers = Application::Get().GetLayers();
                layers.clear();
                auto* layer = new FrameBuffersLayer("Framebuffers");
                layer->Init();
                layers.push_back(layer);
            }
            if (ImGui::Button("Sponza"))
            {
                std::vector<Layer*>& layers = Application::Get().GetLayers();
                layers.clear();
                auto* layer = new SponzaLayer("Sponza");
                layer->Init();
                layers.push_back(layer);
            }
            ImGui::End();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
