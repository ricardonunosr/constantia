#include "sponza.h"

#include "model.h"

#include "application.h"
#include "camera.h"
#include "imgui.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::unique_ptr<Camera> SponzaLayer::m_camera = nullptr;
std::unique_ptr<Camera> SponzaLayer::m_second_camera = nullptr;

SponzaLayer::SponzaLayer(const std::string& name) : Layer(name)
{
    std::string base_path_assets = "./data/";
    m_sponza = std::make_unique<Model>(base_path_assets + "sponza/sponza.obj");
    m_light = std::make_unique<Model>(base_path_assets + "cube/cube.obj");
    m_shader =
        std::make_unique<Shader>(base_path_assets + "shaders/basic.vert", base_path_assets + "shaders/basic.frag");
    m_light_shader =
        std::make_unique<Shader>(base_path_assets + "shaders/light.vert", base_path_assets + "shaders/light.frag");

    m_camera = std::make_unique<Camera>();
    m_second_camera = std::make_unique<Camera>();

    // TODO: find a better way to define this
    glfwSetCursorPosCallback(Application::get().get_window().get_native_window(), mouse_callback);
    glfwSetMouseButtonCallback(Application::get().get_window().get_native_window(), mouse_button_callback);

    int width = Application::get().get_window().get_width();
    int height = Application::get().get_window().get_height();
    // Main Texture Framebuffer
    //-------------------------
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glGenTextures(1, &m_texture_colorbuffer);
    glBindTexture(GL_TEXTURE_2D, m_texture_colorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_colorbuffer, 0);

    // Generate RenderBuffer for depth and/or stencil attachments
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        spdlog::error("Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SponzaLayer::~SponzaLayer() = default;

void SponzaLayer::init()
{
}
void SponzaLayer::de_init()
{
}

void SponzaLayer::on_ui_render(float delta_time)
{
    // ImGui::ShowDemoWindow();
    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0.0f,0.0f));
    if (ImGui::Begin("Second Camera"))
    {
        ImVec2 window_size = ImGui::GetWindowSize();
        ImGui::Image((ImTextureID)m_texture_colorbuffer, window_size, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
    // ImGui::PopStyleVar();

    if (ImGui::Begin("Metrics/Debugger"))
    {
        auto& metrics = Application::get().get_metrics();
        ImGui::Text("Application average %.3f ms/frame (%.3f FPS)", delta_time * 1000.0f,
                    1000.0f / (1000.0f * delta_time));
        ImGui::Text("%d vertices, %d indices (%d triangles)", metrics.vertex_count, metrics.indices_count,
                    metrics.indices_count / 3);
        ImGui::Separator();

        ImGui::End();
    }
}

void SponzaLayer::update(float delta_time)
{
    m_camera->update(Application::get().get_window().get_native_window(), delta_time);

    float light_x = 2.0f * sin(glfwGetTime());
    float light_y = 1.0f;
    float light_z = 1.5f * cos(glfwGetTime());
    glm::vec3 light_pos = glm::vec3(light_x, light_y, light_z);

    m_shader->bind();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    Frustum frustum = create_frustum_from_camera(*m_camera, 1280.0f / 720.0f, 45.0f, 0.1f, 1000.0f);
    unsigned int total = 0;
    unsigned int display = 0;

    m_shader->set_uniform_mat4("projection", m_camera->m_projection);
    m_shader->set_uniform_mat4("view", m_camera->view_matrix());
    m_shader->set_uniform3f("viewPos", m_camera->m_position);
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

    glViewport(0, 0, 1920, 1080);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_shader->set_uniform_mat4("projection", m_second_camera->m_projection);
    m_shader->set_uniform_mat4("view", m_second_camera->view_matrix());
    m_shader->set_uniform3f("viewPos", m_second_camera->m_position);
    m_shader->set_uniform1f("material.shininess", 64.0f);

    m_shader->set_uniform3f("light.position", light_pos);
    m_shader->set_uniform3f("light.ambient", 1.0f, 1.0f, 1.0f);
    m_shader->set_uniform3f("light.diffuse", 1.0f, 1.0f, 1.0f);
    m_shader->set_uniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    m_shader->set_uniform1f("light.constant", 1.0f);
    m_shader->set_uniform1f("light.linear", 0.09f);
    m_shader->set_uniform1f("light.quadratic", 0.032f);

    model = glm::scale(model, glm::vec3(0.02f));
    m_shader->set_uniform_mat4("model", model);
    m_sponza->draw(frustum, model, *m_shader, display, total);

    light_transform = glm::translate(light_transform, light_pos);
    light_transform = glm::scale(light_transform, glm::vec3(0.2f));
    m_light_shader->set_uniform_mat4("model", light_transform);
    m_light->draw(frustum, model, *m_light_shader, display, total);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
