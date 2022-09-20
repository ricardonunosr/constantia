#include "types.h"

Frustum create_frustum_from_camera(const Camera& camera, float aspect, float fov_y, float z_near, float z_far)
{
    Frustum frustum;
    const float half_v_side = z_far * tanf(fov_y * .5f);
    const float half_h_side = half_v_side * aspect;
    const glm::vec3 front_mult_far = z_far * camera.m_front;

    frustum.near_face = {camera.m_position + z_near * camera.m_front, camera.m_front};
    frustum.far_face = {camera.m_position + front_mult_far, -camera.m_front};
    frustum.right_face = {camera.m_position, glm::cross(camera.m_up, front_mult_far + camera.m_right * half_h_side)};
    frustum.left_face = {camera.m_position, glm::cross(front_mult_far - camera.m_right * half_h_side, camera.m_up)};
    frustum.top_face = {camera.m_position, glm::cross(camera.m_right, front_mult_far - camera.m_up * half_v_side)};
    frustum.bottom_face = {camera.m_position, glm::cross(front_mult_far + camera.m_up * half_v_side, camera.m_right)};

    return frustum;
}