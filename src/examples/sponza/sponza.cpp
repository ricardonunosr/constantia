#include "sponza.h"
#include "renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char** argv)
{
    std::unique_ptr<SponzaApplication> app = std::make_unique<SponzaApplication>(WIDTH, HEIGHT, "Sponza");
    app->Run();
}

SponzaLayer::SponzaLayer(const std::string& name) : Layer(name)
{
}

SponzaLayer::~SponzaLayer() = default;

void SponzaLayer::Init()
{
    std::string basePathAssets = "../../data/";
    sponza = std::make_unique<Model>(basePathAssets + "sponza/sponza.obj");
    light = std::make_unique<Model>(basePathAssets + "cube/cube.obj");
    shader = std::make_unique<Shader>(basePathAssets + "shaders/basic.vert", basePathAssets + "shaders/basic.frag");
    light_shader =
        std::make_unique<Shader>(basePathAssets + "shaders/light.vert", basePathAssets + "shaders/light.frag");
}
void SponzaLayer::DeInit()
{
}
void SponzaLayer::Update(float delta_time)
{
    Renderer::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    float lightX = 2.0f * sin(glfwGetTime());
    float lightY = 1.0f;
    float lightZ = 1.5f * cos(glfwGetTime());
    glm::vec3 lightPos = glm::vec3(lightX, lightY, lightZ);

    shader->Bind();

    Camera& camera = Application::Get().GetCamera();
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

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.02f));
    shader->SetUniformMat4("model", model);
    sponza->Draw(*shader);

    glm::mat4 lightTransform = glm::mat4(1.0f);
    lightTransform = glm::translate(lightTransform, lightPos);
    lightTransform = glm::scale(lightTransform, glm::vec3(0.2f));
    light_shader->SetUniformMat4("model", lightTransform);
    light->Draw(*light_shader);
}
