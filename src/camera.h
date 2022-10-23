#pragma once

#include "idk_math.h"
#include "memory.h"

struct GLFWwindow;

struct Camera
{
    idk_mat4 projection;
    idk_vec3 position;
    idk_vec3 forward;
    idk_vec3 right;
    idk_vec3 up;
    bool enabled;
    bool first_mouse;
    float last_x;
    float last_y;
    float yaw;
    float pitch;
};

Camera* create_camera(Arena* arena);
void update(GLFWwindow* window, float delta_time, Camera* camera);
void handle_mouse_move(double xpos, double ypos, Camera* camera);
idk_mat4 view_matrix(Camera* camera);
