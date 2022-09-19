#include "editor.h"
#include "core.h"

#include "application.h"
#include "examples/framebuffers/framebuffers.h"
#include "examples/ray-tracing/ray_tracing.h"
#include "examples/sponza/sponza.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static bool examples = true;

std::vector<Layer*>& layers = Application::get().get_layers();
FrameBuffersLayer* framebuffers = nullptr;
SponzaLayer* sponza = nullptr;
RayTracingLayer* raytracing = nullptr;

void im_gui_init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    framebuffers = new FrameBuffersLayer("FrameBuffers");
    sponza = new SponzaLayer("Sponza");
}

void im_gui_cleanup()
{
    delete framebuffers;
    delete sponza;
    delete raytracing;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void editor_im_gui_render(bool editor)
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    if (editor)
    {
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        ImGui::Begin("Scene");
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
        if (ImGui::Button("RayTracing"))
        {
            layers.clear();
            layers.push_back(raytracing);
        }
        ImGui::End();
    }
}
