#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

    const glm::vec3& GetCameraPosition() const
    {
        return cameraPos;
    }

    bool GetEditorFlag()
    {
        return editor;
    }

  private:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    float lastX = 1280.0f / 2.0;
    float lastY = 720.0 / 2.0;
    float fov = 45.0f;

    bool enabled = true;
    bool editor = false;

    bool firstMouse = true;
    float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing
                        // to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
};