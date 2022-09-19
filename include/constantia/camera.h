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

    glm::mat4 get_view_matrix()
    {
        return m_view_matrix;
    }

    glm::mat4 get_projection_matrix()
    {
        return m_projection_matrix;
    }

    [[nodiscard]] const glm::vec3& get_camera_position() const
    {
        return m_camera_pos;
    }

    [[nodiscard]] bool get_editor_flag() const
    {
        return m_editor;
    }

  private:
    glm::vec3 m_camera_pos{};
    glm::vec3 m_camera_front{};
    glm::vec3 m_camera_up{};
    glm::mat4 m_view_matrix{};
    glm::mat4 m_projection_matrix{};

    float m_last_x = 1280.0f / 2.0;
    float m_last_y = 720.0 / 2.0;
    float m_fov = 45.0f;

    bool m_enabled = false;
    bool m_editor = true;

    bool m_first_mouse = true;
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
};
