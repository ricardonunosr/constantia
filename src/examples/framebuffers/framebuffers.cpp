#include "framebuffers.h"

#include "renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char** argv)
{
    std::unique_ptr<FrameBuffersApplication> app =
        std::make_unique<FrameBuffersApplication>(WIDTH, HEIGHT, "Framebuffers");
    app->Run();
}

FrameBuffersLayer::FrameBuffersLayer(const std::string& name) : Layer(name)
{
}
FrameBuffersLayer::~FrameBuffersLayer()
{
}
void FrameBuffersLayer::Init()
{
    std::string basePathAssets = "../../data/";
    cube = std::make_unique<Model>(basePathAssets + "cube/cube.obj");
    light = std::make_unique<Model>(basePathAssets + "cube/cube.obj");
    outline_shader =
        std::make_unique<Shader>(basePathAssets + "shaders/light.vert", basePathAssets + "shaders/outline.frag");
    post_processing_shader = std::make_unique<Shader>(basePathAssets + "shaders/postprocessing.vert",
                                                      basePathAssets + "shaders/postprocessing.frag");
    shader = std::make_unique<Shader>(basePathAssets + "shaders/basic.vert", basePathAssets + "shaders/basic.frag");
    light_shader =
        std::make_unique<Shader>(basePathAssets + "shaders/light.vert", basePathAssets + "shaders/light.frag");

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate texture
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
        spdlog::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {// positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    vao = std::make_unique<VertexArray>();
    VertexBufferLayout layout = {{"aPos", DataType::Float2}, {"aTexCoords", DataType::Float2}};
    std::unique_ptr<VertexBuffer> vbo = std::make_unique<VertexBuffer>(layout, &quadVertices[0], sizeof(quadVertices));

    vao->AddBuffer(*vbo);
    vao->Unbind();
}
void FrameBuffersLayer::DeInit()
{
}
void FrameBuffersLayer::Update(float delta_time)
{
    // First pass
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    Renderer::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    float lightX = 2.0f * sin(glfwGetTime());
    float lightY = 1.0f;
    float lightZ = 1.5f * cos(glfwGetTime());
    glm::vec3 lightPos = glm::vec3(lightX, lightY, lightZ);

    Camera& camera = Application::Get().GetCamera();
    shader->Bind();
    shader->SetUniformMat4("projection", camera.GetProjectionMatrix());
    shader->SetUniformMat4("view", camera.GetViewMatrix());
    shader->SetUniform3f("viewPos", camera.GetCameraPosition());
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
    shader->SetUniformMat4("model", model);
    cube->Draw(*shader);

    light_shader->Bind();
    light_shader->SetUniformMat4("projection", camera.GetProjectionMatrix());
    light_shader->SetUniformMat4("view", camera.GetViewMatrix());

    glm::mat4 light_transform = glm::mat4(1.0f);
    light_transform = glm::translate(light_transform, lightPos);
    light_transform = glm::scale(light_transform, glm::vec3(0.2f));
    light_shader->SetUniformMat4("model", light_transform);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    light->Draw(*light_shader);

    // Scaled(outline)
    outline_shader->Bind();
    outline_shader->SetUniformMat4("projection", camera.GetProjectionMatrix());
    outline_shader->SetUniformMat4("view", camera.GetViewMatrix());

    glm::mat4 lightOutlineTransform = glm::mat4(1.0f);
    lightOutlineTransform = glm::translate(lightOutlineTransform, lightPos);
    lightOutlineTransform = glm::scale(lightOutlineTransform, glm::vec3(0.24f));
    outline_shader->SetUniformMat4("model", lightOutlineTransform);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    light->Draw(*outline_shader);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);

    // Second Pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    post_processing_shader->Bind();
    vao->Bind();
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}
