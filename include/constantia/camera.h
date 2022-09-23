#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

class Camera
{
  public:
    Camera();

    void update(GLFWwindow* window, float delta_time);
    void handle_mouse_move(double xpos, double ypos);
    glm::mat4 view_matrix();

    glm::mat4 m_projection{};
    glm::vec3 m_position{};
    glm::vec3 m_forward{};
    glm::vec3 m_right{};

  private:
    bool m_first_mouse = true;
    float m_last_x = 1920.0f / 2.0f;
    float m_last_y = 1080.0f / 2.0f;
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
};
