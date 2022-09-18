#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

class Camera
{
  public:
    Camera();

    void ProcessInput(GLFWwindow* window, float deltaTime);
    void MoveCamera(double xpos, double ypos);
    void Zoom(double xoffset, double yoffset);

    glm::mat4 GetViewMatrix()
    {
        return viewMatrix;
    }

    glm::mat4 GetProjectionMatrix()
    {
        return projectionMatrix;
    }

    [[nodiscard]] const glm::vec3& GetCameraPosition() const
    {
        return cameraPos;
    }

    [[nodiscard]] bool GetEditorFlag() const
    {
        return editor;
    }

  private:
    glm::vec3 cameraPos{};
    glm::vec3 cameraFront{};
    glm::vec3 cameraUp{};
    glm::mat4 viewMatrix{};
    glm::mat4 projectionMatrix{};

    float lastX = 1280.0f / 2.0;
    float lastY = 720.0 / 2.0;
    float fov = 45.0f;

    bool enabled = false;
    bool editor = true;

    bool firstMouse = true;
    float yaw = 0.0f;
    float pitch = 0.0f;
};
