#include <glad/gl.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <imgui.h>
#include <iostream>
#include <spdlog/spdlog.h>
// TODO(ricardo): This can dissapear most likely
#include "layer.h"

//#include "examples/framebuffers/framebuffers.h"
#include "examples/sponza/sponza.h"

#define WIDTH 1920
#define HEIGHT 1080

struct Metrics
{
    uint32_t total_allocated = 0;
    uint32_t total_freed = 0;
    uint32_t vertex_count;
    uint32_t indices_count;
    uint32_t current_usage() const
    {
        return total_allocated - total_freed;
    }
};

struct Application
{
    int width = 800;
    int height = 600;
    GLFWwindow* window;
    float delta_time = 0;
    float last_frame = 0;
    std::vector<Layer*> layers;
};

// Globals
static Metrics metrics;
static Application app;
SponzaLayer* sponza = nullptr;
// FrameBuffersLayer* framebuffers = nullptr;

#if 0
void* operator new(size_t size)
{
    std::cout << "Allocating " << size << "bytes\n";
    metrics.total_allocated += size;

    return malloc(size);
}

void operator delete(void* memory, size_t size)
{
    std::cout << "Freeing " << size << "bytes\n";
    metrics.total_freed += size;
    free(memory);
}

void operator delete(void* memory)
{
    free(memory);
}
#endif

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

// TODO(ricardo): maybe instead of one translation unit i just pass the app to the layer.
// Anyways the concept of layer needs to be revisited anyway so this will do
//#include "examples/framebuffers/framebuffers.cpp"
#include "examples/sponza/sponza.cpp"

int main(int /*argc*/, char** /*argv*/)
{
    if (glfwInit() == 0)
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    app.window = glfwCreateWindow(WIDTH, HEIGHT, "Constantia", nullptr, nullptr);
    if (app.window == nullptr)
    {
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(app.window, framebuffer_size_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(app.window);
    // Disable V-Sync
    // glfwSwapInterval(0);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        spdlog::error("Failed to initialize OpenGL context");
        return -1;
    }

    // Successfully loaded OpenGL
    spdlog::info("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    spdlog::info("Current working directory: {}", std::filesystem::current_path().c_str());
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(app.window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
#if 0
    framebuffers = new FrameBuffersLayer("FrameBuffers");
    app.layers.push_back(framebuffers);
#endif
#if 1
    sponza = new SponzaLayer("Sponza");
    app.layers.push_back(sponza);
#endif
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);

    std::cout << "Startup Usage " << metrics.current_usage() << " bytes\n";
    // Main Loop
    while (!(glfwWindowShouldClose(app.window) != 0))
    {
        metrics.total_allocated = 0;
        metrics.total_freed = 0;
        float current_frame = static_cast<float>(glfwGetTime());
        app.delta_time = current_frame - app.last_frame;
        app.last_frame = current_frame;

        for (Layer* layer : app.layers)
            layer->update(app.delta_time);

        // UI Scope
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            for (Layer* layer : app.layers)
                layer->on_ui_render(app.delta_time);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        std::cout << "Allocated during frame " << metrics.current_usage() << " bytes\n";
        glfwPollEvents();
        glfwSwapBuffers(app.window);
    }
    // Shutdown
    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}