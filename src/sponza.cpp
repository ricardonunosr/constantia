#include <glad/gl.h>

#include "imgui.h"

#include <GLFW/glfw3.h>
#include <iostream>

struct SponzaShader
{
  OpenGLProgramCommon common;

  GLint light_position;
  GLint light_ambient;
  GLint light_diffuse;
  GLint light_specular;
  GLint light_constant;
  GLint light_linear;
  GLint light_quadratic;
};

static Camera* camera = nullptr;
static Camera* second_camera = nullptr;

static void mouse_callback(GLFWwindow* /*window*/, double xpos, double ypos)
{
  handle_mouse_move(xpos, ypos, camera);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
  {
    camera->enabled = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  else
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    camera->enabled = false;
  }
}

struct Sponza
{
  Model* light;
  Model* sponza;
  SponzaShader* shader;
  OpenGLProgramCommon* light_shader;
  unsigned int framebuffer;
  unsigned int rbo;
  unsigned int texture_colorbuffer;
};

static Arena* arena = arena_alloc(Megabytes(30));

Sponza* sponza = (Sponza*)new Sponza;

void init()
{
  Arena* temp = arena_alloc(Megabytes(10));
  std::string base_path_assets = "./data/";

  sponza->sponza = create_model(arena, base_path_assets + "sponza/sponza.obj");
  sponza->light = create_model(arena, base_path_assets + "cube/cube.obj");

  // Sponza
  std::string vertex_shader_path = base_path_assets + "shaders/basic.vert";
  std::string fragment_shader_path = base_path_assets + "shaders/basic.frag";
  ReadEntireFile vertex_shader_source = read_entire_file(temp, vertex_shader_path.c_str());
  ReadEntireFile fragment_shader_source = read_entire_file(temp, fragment_shader_path.c_str());

  sponza->shader = (SponzaShader*)opengl_create_shader(arena, vertex_shader_source.content, fragment_shader_source.content);
  SponzaShader* shader = sponza->shader;
#define SHADER_PUSH_GET_UNIFORM(uniform_location,arena,program_id,uniform_name) arena_push(arena, sizeof(GLint)); \
                                                                                uniform_location = glGetUniformLocation(program_id, uniform_name);
  SHADER_PUSH_GET_UNIFORM(shader->light_position, arena, shader->common.program_id, "light.position");
  SHADER_PUSH_GET_UNIFORM(shader->light_ambient, arena, shader->common.program_id, "light.ambient");
  SHADER_PUSH_GET_UNIFORM(shader->light_diffuse, arena, shader->common.program_id, "light.diffuse");
  SHADER_PUSH_GET_UNIFORM(shader->light_specular, arena, shader->common.program_id, "light.specular");
  SHADER_PUSH_GET_UNIFORM(shader->light_constant, arena, shader->common.program_id, "light.constant");
  SHADER_PUSH_GET_UNIFORM(shader->light_linear, arena, shader->common.program_id, "light.linear");
  SHADER_PUSH_GET_UNIFORM(shader->light_quadratic, arena, shader->common.program_id, "light.quadratic");

  // Light
  std::string vertex_shader_light_path = base_path_assets + "shaders/light.vert";
  std::string fragment_shader_light_path = base_path_assets + "shaders/light.frag";
  ReadEntireFile vertex_shader_light_source = read_entire_file(temp, vertex_shader_light_path.c_str());
  ReadEntireFile fragment_shader_light_source = read_entire_file(temp, fragment_shader_light_path.c_str());

  sponza->light_shader = opengl_create_shader(arena, vertex_shader_light_source.content, fragment_shader_light_source.content);

  camera = create_camera(arena);
  second_camera = create_camera(arena);

  glfwSetCursorPosCallback(app.window, mouse_callback);
  glfwSetMouseButtonCallback(app.window, mouse_button_callback);

  //-------------------------
  // Main Texture Framebuffer
  glGenFramebuffers(1, &sponza->framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, sponza->framebuffer);

  glGenTextures(1, &sponza->texture_colorbuffer);
  glBindTexture(GL_TEXTURE_2D, sponza->texture_colorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // attach it to currently bound framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sponza->texture_colorbuffer, 0);

  // Generate RenderBuffer for depth and/or stencil attachments
  glGenRenderbuffers(1, &sponza->rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, sponza->rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sponza->rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer is not complete!\n");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glUseProgram(shader->common.program_id);
  glUniform1f(shader->common.material_shininess, 64.0f);
  glUniform3f(shader->light_ambient, 1.0f, 1.0f, 1.0f);
  glUniform3f(shader->light_diffuse, 1.0f, 1.0f, 1.0f);
  glUniform3f(shader->light_specular, 1.0f, 1.0f, 1.0f);
  glUniform1f(shader->light_constant, 1.0f);
  glUniform1f(shader->light_linear, 0.09f);
  glUniform1f(shader->light_quadratic, 0.032f);
  glUseProgram(0);
}

void deinit()
{
  glDeleteProgram(sponza->shader->common.program_id);
  glDeleteProgram(sponza->light_shader->program_id);
}

void ui_render(float delta_time)
{
  //    ImGui::ShowDemoWindow();
  //    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

  //    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  if (ImGui::Begin("Second Camera"))
  {
    ImVec2 window_size = ImGui::GetWindowSize();
    ImGui::Image(reinterpret_cast<ImTextureID>(sponza->texture_colorbuffer), window_size, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
  }
  //    ImGui::PopStyleVar();

  if (ImGui::Begin("Metrics/Debugger"))
  {
    ImGui::Text("Application average %.3f ms/frame (%.3f FPS)", delta_time * 1000.0f,
        1000.0f / (1000.0f * delta_time));
    ImGui::Text("%d vertices, %d indices (%d triangles)", metrics.vertex_count, metrics.indices_count,
        metrics.indices_count / 3);
    ImGui::Separator();

    ImGui::End();
  }
}

void update_and_render(float delta_time)
{
  update(app.window, delta_time, camera);

  float light_x = 2.0f * sin(glfwGetTime());
  float light_y = 1.0f;
  float light_z = 1.5f * cos(glfwGetTime());
  idk_vec3 light_pos = idk_vec3f(light_x, light_y, light_z);

  idk_mat4 model = idk_mat4f(1.0f);
  model = idk_scale(model, idk_vec3fv(0.02f));

  idk_mat4 light_transform =idk_mat4f(1.0f);
  light_transform = idk_translate(light_transform, light_pos);
  light_transform = idk_scale(light_transform, idk_vec3fv(0.2f));
  SponzaShader* shader = sponza->shader;
  OpenGLProgramCommon* light_shader = sponza->light_shader;

  {
    // Primary Framebuffer
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(shader->common.program_id);
    glUniformMatrix4fv(shader->common.projection, 1, false, camera->projection.elements[0]);
    glUniformMatrix4fv(shader->common.view, 1, false, view_matrix(camera).elements[0]);
    glUniform3fv(shader->common.view_pos, 1, &camera->position[0]);
    glUniform3fv(shader->light_position, 1, &light_pos[0]);

    glUniform1f(shader->common.material_shininess, 64.0f);
    glUniform3f(shader->light_ambient, 1.0f, 1.0f, 1.0f);
    glUniform3f(shader->light_diffuse, 1.0f, 1.0f, 1.0f);
    glUniform3f(shader->light_specular, 1.0f, 1.0f, 1.0f);
    glUniform1f(shader->light_constant, 1.0f);
    glUniform1f(shader->light_linear, 0.09f);
    glUniform1f(shader->light_quadratic, 0.032f);

    glUniformMatrix4fv(shader->common.model, 1, GL_FALSE, model.elements[0]);
    draw(sponza->sponza,model, (OpenGLProgramCommon*)shader);

    glUseProgram(light_shader->program_id);
    glUniformMatrix4fv(light_shader->model, 1, GL_FALSE, light_transform.elements[0]);
    draw(sponza->light,model, light_shader);
  }

  {
    // Second Framebuffer
    glViewport(0, 0, 1920, 1080);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindFramebuffer(GL_FRAMEBUFFER, sponza->framebuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(shader->common.program_id);
    glUniformMatrix4fv(shader->common.projection, 1, false, second_camera->projection.elements[0]);
    glUniformMatrix4fv(shader->common.view, 1, false, view_matrix(second_camera).elements[0]);
    glUniform3fv(shader->common.view_pos, 1, &camera->position[0]);
    glUniform3fv(shader->light_position, 1, &light_pos[0]);

    glUniform1f(shader->common.material_shininess, 64.0f);
    glUniform3f(shader->light_ambient, 1.0f, 1.0f, 1.0f);
    glUniform3f(shader->light_diffuse, 1.0f, 1.0f, 1.0f);
    glUniform3f(shader->light_specular, 1.0f, 1.0f, 1.0f);
    glUniform1f(shader->light_constant, 1.0f);
    glUniform1f(shader->light_linear, 0.09f);
    glUniform1f(shader->light_quadratic, 0.032f);

    draw(sponza->sponza,model, (OpenGLProgramCommon*)shader);

    glUseProgram(light_shader->program_id);
    glUniformMatrix4fv(light_shader->model, 1, GL_FALSE, light_transform.elements[0]);
    draw(sponza->light,model, light_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
  }
}
