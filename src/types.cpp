#include "types.h"

constexpr glm::vec3 K_WORLD_UP(0.0f, 1.0f, 0.0f);

Frustum create_frustum_from_camera(const Camera& camera, float aspect, float fov_y, float z_near, float z_far)
{
    Frustum frustum;
    const float half_v_side = z_far * tanf(fov_y * .5f);
    const float half_h_side = half_v_side * aspect;
    const glm::vec3 front_mult_far = z_far * camera.m_forward;

    frustum.near_face = {camera.m_position + z_near * camera.m_forward, camera.m_forward};
    frustum.far_face = {camera.m_position + front_mult_far, -camera.m_forward};
    frustum.right_face = {camera.m_position, glm::cross(K_WORLD_UP, front_mult_far + camera.m_right * half_h_side)};
    frustum.left_face = {camera.m_position, glm::cross(front_mult_far - camera.m_right * half_h_side, K_WORLD_UP)};
    frustum.top_face = {camera.m_position, glm::cross(camera.m_right, front_mult_far - K_WORLD_UP * half_v_side)};
    frustum.bottom_face = {camera.m_position, glm::cross(front_mult_far + K_WORLD_UP * half_v_side, camera.m_right)};

    return frustum;
}