#pragma once

#include "index_buffer.h"
#include "shader.h"
#include "texture.h"
#include "vertex_array.h"
#include <glm/glm.hpp>
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
    }

    void draw(Shader& shader);

  private:
    std::vector<MeshMaterialGroup> m_meshes = {};
    std::string m_directory;

    void load_model(const std::string& path);
};
