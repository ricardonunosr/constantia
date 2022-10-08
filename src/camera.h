#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

struct Camera
{
    glm::mat4 projection;
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    bool enabled;
    bool first_mouse;
    float last_x;
    float last_y;
    float yaw;
    float pitch;
};

void create_camera(Camera* camera);
void update(GLFWwindow* window, float delta_time, Camera* camera);
void handle_mouse_move(double xpos, double ypos, Camera* camera);
glm::mat4 view_matrix(Camera* camera);
