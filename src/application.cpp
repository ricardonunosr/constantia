#include "application.h"
#include "camera.h"
#include "editor.h"
#include "model.h"
#include "renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Application* Application::instance = nullptr;

Shader* shader;
Model* cube;
Camera* camera;

void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                     const void* userParam)
{
    spdlog::error("GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
                  (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->MoveCamera(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->Zoom(xoffset, yoffset);
}

Application::Application(int width, int height, const std::string& name)
{
    instance = this;

    window = std::make_unique<Window>(width, height, name);
    glfwSetScrollCallback(window->GetNativeWindow(), scroll_callback);
    glfwSetCursorPosCallback(window->GetNativeWindow(), mouse_callback);

    Init();

    shader = new Shader("../../../data/shaders/basic.vert", "../../../data/shaders/basic.frag");
    cube = new Model("../../../data/cube/cube.obj");
    camera = new Camera();
}

Application::~Application()
{
    Cleanup();
}

void Application::Init()
{
    ImGuiInit(window->GetNativeWindow());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, 0);
}

void Application::Update()
{
    while (!window->ShouldClose())
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera->ProcessInput(window->GetNativeWindow(), deltaTime);
        Renderer::SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glm::mat4 model = glm::mat4(1.0f);

        shader->Bind();

        shader->SetUniformMat4("projection", camera->GetProjectionMatrix());

        shader->SetUniformMat4("view", camera->GetViewMatrix());

        shader->SetUniformMat4("model", model);

        cube->Draw(*shader);

        bool editor = camera->GetEditorFlag();
        EditorImGuiRender(editor);

        window->Update();
    }
}

void Application::Cleanup()
{
    ImGuiCleanup();
}