#include "application.h"
#include "camera.h"
#include "editor.h"
#include "model.h"
#include "renderer.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Application* Application::instance = nullptr;

Shader* shader;
Shader* lightShader;
Model* cube;
Camera* camera;
Model* light;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera->MoveCamera(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->Zoom(xoffset, yoffset);
}

void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                     const void* userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        spdlog::error("GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
                      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        spdlog::warn("GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
                     (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        spdlog::debug("GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
                      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
        break;
    default:
        spdlog::trace("GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
                      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

Application::Application(int width, int height, const std::string& name)
{
    instance = this;

    window = std::make_unique<Window>(width, height, name);
    glfwSetScrollCallback(window->GetNativeWindow(), scroll_callback);
    glfwSetCursorPosCallback(window->GetNativeWindow(), mouse_callback);

    Init();

    // Move to spdlog
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    shader = new Shader("../../../data/shaders/basic.vert", "../../../data/shaders/basic.frag");
    lightShader = new Shader("../../../data/shaders/light.vert", "../../../data/shaders/light.frag");
    cube = new Model("../../../data/sponza/sponza.obj");
    light = new Model("../../../data/cube/cube.obj");
    camera = new Camera();
}

Application::~Application()
{
    Cleanup();
}

void Application::Init()
{
    ImGuiInit(window->GetNativeWindow());

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, 0);
    glEnable(GL_DEPTH_TEST);
}

void Application::Update()
{
    while (!window->ShouldClose())
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera->ProcessInput(window->GetNativeWindow(), deltaTime);
        Renderer::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        float lightX = 2.0f * sin(glfwGetTime());
        float lightY = 1.0f;
        float lightZ = 1.5f * cos(glfwGetTime());
        glm::vec3 lightPos = glm::vec3(lightX, lightY, lightZ);

        shader->Bind();

        shader->SetUniformMat4("projection", camera->GetProjectionMatrix());

        shader->SetUniformMat4("view", camera->GetViewMatrix());

        shader->SetUniform3f("viewPos", camera->GetCameraPosition());
        shader->SetUniform1f("material.shininess", 64.0f);

        shader->SetUniform3f("light.position", lightPos);
        shader->SetUniform3f("light.ambient", 1.0f, 1.0f, 1.0f);
        shader->SetUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
        shader->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

        shader->SetUniform1f("light.constant", 1.0f);
        shader->SetUniform1f("light.linear", 0.09f);
        shader->SetUniform1f("light.quadratic", 0.032f);

        lightShader->Bind();
        lightShader->SetUniformMat4("projection", camera->GetProjectionMatrix());
        lightShader->SetUniformMat4("view", camera->GetViewMatrix());

        glm::mat4 lightTransform = glm::mat4(1.0f);
        lightTransform = glm::translate(lightTransform, lightPos);
        lightTransform = glm::scale(lightTransform, glm::vec3(0.2f));
        lightShader->SetUniformMat4("model", lightTransform);

        light->Draw(*lightShader);

        shader->Bind();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.02f));
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
