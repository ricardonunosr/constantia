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
SponzaLayer* sponza = nullptr;
// FrameBuffersLayer* framebuffers = nullptr;
// RayTracingLayer* raytracing = nullptr;

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
#ifdef FRAMEBUFFERS_EXAMPLE
    // framebuffers = new FrameBuffersLayer("FrameBuffers");
#endif
#if 1
    sponza = new SponzaLayer("Sponza");
    layers.push_back(sponza);
#endif
}

void im_gui_cleanup()
{
    // delete framebuffers;
    delete sponza;
    // delete raytracing;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
