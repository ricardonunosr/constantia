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
Shader* outlineShader;
Shader* postprocessing;
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

    shader = new Shader("./data/shaders/basic.vert", "./data/shaders/basic.frag");
    lightShader = new Shader("./data/shaders/light.vert", "./data/shaders/light.frag");
    outlineShader = new Shader("./data/shaders/light.vert", "./data/shaders/outline.frag");
    postprocessing =
        new Shader("./data/shaders/postprocessing.vert", "./data/shaders/postprocessing.frag");
    cube = new Model("./data/cube/cube.obj");
    light = new Model("./data/cube/cube.obj");
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
    //glDebugMessageCallback(messageCallback, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
}

void Application::Update()
{
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {// positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    std::unique_ptr<VertexArray> vao = std::make_unique<VertexArray>();
    VertexBufferLayout layout = {{"aPos", DataType::Float2}, {"aTexCoords", DataType::Float2}};
    std::unique_ptr<VertexBuffer> vbo = std::make_unique<VertexBuffer>(layout, &quadVertices[0], sizeof(quadVertices));

    vao->AddBuffer(*vbo);
    vao->Unbind();

    while (!window->ShouldClose())
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera->ProcessInput(window->GetNativeWindow(), deltaTime);
        // First pass
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
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

        glStencilMask(0x00);
        shader->Bind();
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(0.02f));
        shader->SetUniformMat4("model", model);
        cube->Draw(*shader);

        lightShader->Bind();
        lightShader->SetUniformMat4("projection", camera->GetProjectionMatrix());
        lightShader->SetUniformMat4("view", camera->GetViewMatrix());

        glm::mat4 lightTransform = glm::mat4(1.0f);
        lightTransform = glm::translate(lightTransform, lightPos);
        lightTransform = glm::scale(lightTransform, glm::vec3(0.2f));
        lightShader->SetUniformMat4("model", lightTransform);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        light->Draw(*lightShader);

        // Scaled(outline)
        outlineShader->Bind();
        outlineShader->SetUniformMat4("projection", camera->GetProjectionMatrix());
        outlineShader->SetUniformMat4("view", camera->GetViewMatrix());

        glm::mat4 lightOutlineTransform = glm::mat4(1.0f);
        lightOutlineTransform = glm::translate(lightOutlineTransform, lightPos);
        lightOutlineTransform = glm::scale(lightOutlineTransform, glm::vec3(0.24f));
        outlineShader->SetUniformMat4("model", lightOutlineTransform);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        light->Draw(*outlineShader);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        // Second Pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        postprocessing->Bind();
        vao->Bind();
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);

        /*bool editor = camera->GetEditorFlag();
        EditorImGuiRender(editor);*/

        window->Update();
    }
}

void Application::Cleanup()
{
    ImGuiCleanup();
}
