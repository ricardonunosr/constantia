#include "camera.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

constexpr glm::vec3 K_WORLD_UP(0.0f, 1.0f, 0.0f);
constexpr float K_MOUSE_SENSITIVITY = 0.1f;

Camera::Camera()
{
    m_position = glm::vec3(-25.0f, 3.0f, 0.0f);
    m_forward = glm::vec3(1.0f, 0.0f, 0.0f);
    m_projection = glm::perspective(glm::radians(45.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
}

void Camera::update(GLFWwindow* window, float delta_time)
{
    float camera_speed = 3.5f * delta_time; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_position += camera_speed * m_forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_position -= camera_speed * m_forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_position -= glm::normalize(glm::cross(m_forward, K_WORLD_UP)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_position += glm::normalize(glm::cross(m_forward, K_WORLD_UP)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_Q) != 0)
        m_position += camera_speed * K_WORLD_UP;
    if (glfwGetKey(window, GLFW_KEY_E) != 0)
        m_position -= camera_speed * K_WORLD_UP;
}

void Camera::handle_mouse_move(double xpos, double ypos)
{
    if (!m_enabled)
        return;

    // Calculate delta
    if (m_first_mouse)
    {
        m_last_x = xpos;
        m_last_y = ypos;
        m_first_mouse = false;
    }

    float xoffset = xpos - m_last_x;
    float yoffset = m_last_y - ypos;
    m_last_x = xpos;
    m_last_y = ypos;

    m_yaw += xoffset * K_MOUSE_SENSITIVITY;
    m_pitch += yoffset * K_MOUSE_SENSITIVITY;
    m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

    m_forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward.y = sin(glm::radians(m_pitch));
    m_forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(m_forward);
    m_right = glm::normalize(glm::cross(m_forward, K_WORLD_UP));
}

glm::mat4 Camera::view_matrix() const
{
    return glm::lookAt(m_position, m_position + m_forward, K_WORLD_UP);
}