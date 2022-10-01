#pragma once

#include "camera.h"
#include <algorithm>
#include <glm/glm.hpp>

struct Plan
{
    glm::vec3 normal = {0.f, 1.f, 0.f};
    float distance = 0.f;

    Plan() = default;

    Plan(const glm::vec3& p1, const glm::vec3& normal) : normal(glm::normalize(normal)), distance(glm::dot(normal, p1))
    {
    }

    float get_signed_distance_to_plan(const glm::vec3& point) const
    {
        return glm::dot(normal, point) - distance;
    }
};

struct Frustum
{
    Plan top_face;
    Plan bottom_face;
    Plan right_face;
    Plan left_face;
    Plan far_face;
    Plan near_face;
};

struct BoundingVolume
{
    virtual bool is_on_frustum(const Frustum& cam_frustum, const glm::mat4& transform) const = 0;
    virtual bool is_on_forward_plan(const Plan& plan) const = 0;

    bool is_on_frustum(const Frustum& cam_frustum) const
    {
        return (is_on_forward_plan(cam_frustum.left_face) && is_on_forward_plan(cam_frustum.right_face) &&
                is_on_forward_plan(cam_frustum.top_face) && is_on_forward_plan(cam_frustum.bottom_face) &&
                is_on_forward_plan(cam_frustum.near_face) && is_on_forward_plan(cam_frustum.far_face));
    };
};

struct Sphere : public BoundingVolume
{
    glm::vec3 center{0.f, 0.f, 0.f};
    float radius{0.f};

    Sphere(const glm::vec3& center, float radius) : BoundingVolume{}, center(center), radius(radius)
    {
    }

    bool is_on_forward_plan(const Plan& plan) const final
    {
        return plan.get_signed_distance_to_plan(center) > -radius;
    }

    bool is_on_frustum(const Frustum& cam_frustum, const glm::mat4& transform) const final
    {
        const glm::vec3 global_scale = glm::vec3{1.f, 1.f, 1.f};
        const glm::vec3 global_center{transform * glm::vec4(center, 1.f)};
        const float max_scale = std::max(std::max(global_scale.x, global_scale.y), global_scale.z);
        Sphere global_sphere(global_center, radius * (max_scale * 0.5f));

        return (global_sphere.is_on_forward_plan(cam_frustum.left_face) &&
                global_sphere.is_on_forward_plan(cam_frustum.right_face) &&
                global_sphere.is_on_forward_plan(cam_frustum.far_face) &&
                global_sphere.is_on_forward_plan(cam_frustum.near_face) &&
                !global_sphere.is_on_forward_plan(cam_frustum.top_face) &&
                !global_sphere.is_on_forward_plan(cam_frustum.bottom_face));
    }
};

Frustum create_frustum_from_camera(const Camera& camera, float aspect, float fov_y, float z_near, float z_far);