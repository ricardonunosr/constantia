#include <glad/gl.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <imgui.h>
#include <iostream>

#define WIDTH 1920
#define HEIGHT 1080

// Examples functions
void init();
void ui_render(float delta_time);
void update_and_render(float delta_time);
void deinit();

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
};

// Globals
static Metrics metrics;
static Application app;

// Note(ricardo): This is slow af on Windows
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

#include "memory.cpp"
#include "idk_math.h"
#include "camera.cpp"
#include "opengl_renderer.cpp"
#include "model.cpp"
#include "sponza.cpp"

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
    printf("Failed to initialize OpenGL context\n");
    return -1;
  }

  printf("Loaded OpenGL %d.%d\n",GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  printf("Current working directory: %s\n",std::filesystem::current_path().c_str());
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(app.window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glEnable(GL_CULL_FACE);

  init();

  // Main Loop
  while (!(glfwWindowShouldClose(app.window) != 0))
  {
    float current_frame = static_cast<float>(glfwGetTime());
    app.delta_time = current_frame - app.last_frame;
    app.last_frame = current_frame;

    update_and_render(app.delta_time);

    // UI Scope
    {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ui_render(app.delta_time);

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glfwPollEvents();
    glfwSwapBuffers(app.window);
  }
  // Shutdown
  deinit();
  glfwTerminate();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  return 0;
}
