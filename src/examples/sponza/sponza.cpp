#include "sponza.h"

#include "model.h"

#include "application.h"
#include "camera.h"
#include "imgui.h"

#include "renderer.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SponzaLayer::SponzaLayer(const std::string& name) : Layer(name)
{
    std::string base_path_assets = "./data/";
    m_sponza = std::make_unique<Model>(base_path_assets + "sponza/sponza.obj");
    m_light = std::make_unique<Model>(base_path_assets + "cube/cube.obj");
    m_shader =
        std::make_unique<Shader>(base_path_assets + "shaders/basic.vert", base_path_assets + "shaders/basic.frag");
    m_light_shader =
        std::make_unique<Shader>(base_path_assets + "shaders/light.vert", base_path_assets + "shaders/light.frag");
}

SponzaLayer::~SponzaLayer() = default;

void SponzaLayer::init()
{
}
void SponzaLayer::de_init()
{
}

void SponzaLayer::on_ui_render()
{
    ImGui::Begin("test");
    ImGui::End();
}

void SponzaLayer::update(float /*delta_time*/)
{
    Renderer::set_clear_color(0.1f, 0.1f, 0.1f, 1.0f);

    float light_x = 2.0f * sin(glfwGetTime());
    float light_y = 1.0f;
    float light_z = 1.5f * cos(glfwGetTime());
    glm::vec3 light_pos = glm::vec3(light_x, light_y, light_z);

    m_shader->bind();

    Camera& camera = Application::get().get_camera();
    Frustum frustum = create_frustum_from_camera(camera, 1280.0f / 720.0f, 45.0f, 0.1f, 1000.0f);
    unsigned int total = 0;
    unsigned int display = 0;

    m_shader->set_uniform_mat4("projection", camera.m_projection_matrix);
    m_shader->set_uniform_mat4("view", camera.m_view_matrix);
    m_shader->set_uniform3f("viewPos", camera.m_position);
    m_shader->set_uniform1f("material.shininess", 64.0f);

    m_shader->set_uniform3f("light.position", light_pos);
    m_shader->set_uniform3f("light.ambient", 1.0f, 1.0f, 1.0f);
    m_shader->set_uniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    m_shader->set_uniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    m_shader->set_uniform1f("light.constant", 1.0f);
    m_shader->set_uniform1f("light.linear", 0.09f);
    m_shader->set_uniform1f("light.quadratic", 0.032f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.02f));
    m_shader->set_uniform_mat4("model", model);
    m_sponza->draw(frustum, model, *m_shader, display, total);

    glm::mat4 light_transform = glm::mat4(1.0f);
    light_transform = glm::translate(light_transform, light_pos);
    light_transform = glm::scale(light_transform, glm::vec3(0.2f));
    m_light_shader->set_uniform_mat4("model", light_transform);
    m_light->draw(frustum, model, *m_light_shader, display, total);
    spdlog::info("Total Process in CPU: {} / Total send to GPU: {}", total, display);
}
