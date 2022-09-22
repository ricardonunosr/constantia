#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

class Camera
{
  public:
    Camera();

    void process_input(GLFWwindow* window, float delta_time);
    void move_camera(double xpos, double ypos);
    void zoom(double xoffset, double yoffset);

    glm::vec3 m_position{};
    glm::vec3 m_front{};
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    glm::mat4 m_view_matrix{};
    glm::mat4 m_projection_matrix{};

  private:
    float m_last_x = 1920.0f / 2.0;
    float m_last_y = 1080.0f / 2.0;
    float m_fov = 45.0f;

    bool m_first_mouse = true;
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
};
