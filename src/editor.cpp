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

void EditorImGuiRender(bool editor)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showDemo)
        ImGui::ShowDemoWindow(&showDemo);

    if (editor)
    {
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
                auto* layer = new FrameBuffersLayer("Framebuffers");
                layer->Init();
                Application::Get().GetLayers().push_back(layer);
            }
            if (ImGui::Button("Sponza"))
            {
                auto* layer = new SponzaLayer("Sponza");
                layer->Init();
                Application::Get().GetLayers().push_back(layer);
            }
            ImGui::End();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
