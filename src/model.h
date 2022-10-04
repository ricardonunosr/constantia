#pragma once

#include "opengl_renderer.h"
#include "types.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;

    bool operator==(const Vertex& other) const
    {
        return position == other.position && tex_coords == other.tex_coords;
    }
};

class Model
{
    struct MeshMaterialGroup
    {
        std::vector<Vertex> vertices{};
        std::vector<unsigned int> indices{};
        std::vector<Texture> textures{};

        std::unique_ptr<VertexArray> vao;
        std::unique_ptr<VertexBuffer> vbo;
        std::unique_ptr<IndexBuffer> ibo;
    };

  public:
    explicit Model(const std::string& path)
    {
        load_model(path);
        m_bounding_volume = std::make_unique<Sphere>(generate_sphere_bv());
    }

    void draw(const Frustum& frustum, const glm::mat4& transform, OpenGLProgramCommon* shader, unsigned int& display,
              unsigned int& total);

    Sphere generate_sphere_bv()
    {
        glm::vec3 min_aabb = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max_aabb = glm::vec3(std::numeric_limits<float>::min());
        for (auto&& mesh : m_meshes)
        {
            for (auto&& vertex : mesh.vertices)
            {
                min_aabb.x = std::min(min_aabb.x, vertex.position.x);
                min_aabb.y = std::min(min_aabb.y, vertex.position.y);
                min_aabb.z = std::min(min_aabb.z, vertex.position.z);

                max_aabb.x = std::max(max_aabb.x, vertex.position.x);
                max_aabb.y = std::max(max_aabb.y, vertex.position.y);
                max_aabb.z = std::max(max_aabb.z, vertex.position.z);
            }
        }

        return Sphere((max_aabb + min_aabb) * 0.5f, glm::length(min_aabb - max_aabb));
    }

  private:
    std::vector<MeshMaterialGroup> m_meshes = {};
    std::unique_ptr<Sphere> m_bounding_volume;
    std::string m_directory;

    void load_model(const std::string& path);
};
