#include "camera.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

constexpr glm::vec3 K_WORLD_UP(0.0f, 1.0f, 0.0f);
constexpr float K_MOUSE_SENSITIVITY = 0.1f;

void create_camera(Camera* camera)
{
    camera->enabled =false; 
    camera->first_mouse = true;
    camera->last_x = 1920.0f / 2.0f;
    camera->last_y = 1080.0f / 2.0f;
    camera->yaw = 0.0f;
    camera->pitch = 0.0f;
    camera->position = glm::vec3(-25.0f, 3.0f, 0.0f);
    camera->forward = glm::vec3(1.0f, 0.0f, 0.0f);
    camera->projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
}

void update(GLFWwindow* window, float delta_time, Camera* camera)
{
    float camera_speed = 3.5f * delta_time; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->position += camera_speed * camera->forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->position -= camera_speed * camera->forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->position -= glm::normalize(glm::cross(camera->forward, K_WORLD_UP)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->position += glm::normalize(glm::cross(camera->forward, K_WORLD_UP)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_Q) != 0)
        camera->position += camera_speed * K_WORLD_UP;
    if (glfwGetKey(window, GLFW_KEY_E) != 0)
        camera->position -= camera_speed * K_WORLD_UP;
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
    camera->pitch = glm::clamp(camera->pitch, -89.0f, 89.0f);

    camera->forward.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    camera->forward.y = sin(glm::radians(camera->pitch));
    camera->forward.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
    camera->forward = glm::normalize(camera->forward);
    camera->right = glm::normalize(glm::cross(camera->forward, K_WORLD_UP));
}

glm::mat4 view_matrix(Camera* camera)
{
    return glm::lookAt(camera->position, camera->position + camera->forward, K_WORLD_UP);
}
