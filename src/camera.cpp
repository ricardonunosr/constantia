#include "camera.h"
#include <GLFW/glfw3.h>

#define K_MOUSE_SENSITIVITY 0.1f

Camera* create_camera(Arena* arena)
{
  Camera* camera = (Camera*)arena_push(arena, sizeof(Camera));
  camera->enabled =false; 
  camera->first_mouse = true;
  camera->last_x = 1920.0f / 2.0f;
  camera->last_y = 1080.0f / 2.0f;
  camera->yaw = 0.0f;
  camera->pitch = 0.0f;
  camera->position = idk_vec3f(-25.0f, 3.0f, 0.0f);
  camera->forward = idk_vec3f(1.0f, 0.0f, 0.0f);
  camera->up = idk_vec3f(0.0f, 1.0f, 0.0f);
  camera->right = idk_vec3f(0.0f, 0.0f, 1.0f);
  camera->projection = idk_perspective(idk_radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
  return camera;
}

void update(GLFWwindow* window, float delta_time, Camera* camera)
{
  float camera_speed = 3.5f * delta_time; // adjust accordingly
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera->position += camera_speed * camera->forward;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera->position -= camera_speed * camera->forward;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera->position -= idk_normalize_vec3(idk_cross(camera->forward, camera->up)) * camera_speed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera->position += idk_normalize_vec3(idk_cross(camera->forward, camera->up)) * camera_speed;
  if (glfwGetKey(window, GLFW_KEY_Q) != 0)
    camera->position += camera_speed * camera->up;
  if (glfwGetKey(window, GLFW_KEY_E) != 0)
    camera->position -= camera_speed * camera->up;
}

void handle_mouse_move(double xpos, double ypos, Camera* camera)
{
  if (!camera->enabled)
    return;

  // Calculate delta
  if (camera->first_mouse)
  {
    camera->last_x = xpos;
    camera->last_y = ypos;
    camera->first_mouse = false;
  }

  float xoffset = xpos - camera->last_x;
  float yoffset = camera->last_y - ypos;
  camera->last_x = xpos;
  camera->last_y = ypos;

  camera->yaw += xoffset * K_MOUSE_SENSITIVITY;
  camera->pitch += yoffset * K_MOUSE_SENSITIVITY;
  camera->pitch = idk_clamp(camera->pitch, -89.0f, 89.0f);

  camera->forward.x = cos(idk_radians(camera->yaw)) * cos(idk_radians(camera->pitch));
  camera->forward.y = sin(idk_radians(camera->pitch));
  camera->forward.z = sin(idk_radians(camera->yaw)) * cos(idk_radians(camera->pitch));
  camera->forward = idk_normalize_vec3(camera->forward);
  camera->right = idk_normalize_vec3(idk_cross(camera->forward, camera->up));
}

idk_mat4 view_matrix(Camera* camera)
{
  return idk_lookat(camera->position, camera->position + camera->forward, camera->up);
}
