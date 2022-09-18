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
    glm::vec2 texCoords;

    bool operator==(const Vertex& other) const
    {
        return position == other.position && texCoords == other.texCoords;
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
        loadModel(path);
    }

    void Draw(Shader& shader);

  private:
    std::vector<MeshMaterialGroup> meshes = {};
    std::string directory;

    void loadModel(const std::string& path);
};
