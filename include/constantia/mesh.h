#ifndef MESH_H
#define MESH_H

#include "index_buffer.h"
#include "shader.h"
#include "texture.h"
#include "vertex_array.h"
#include <glm/glm.hpp>
#include <memory>
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

class Mesh
{
  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader& shader);

  private:
    std::unique_ptr<VertexArray> vao;
    std::unique_ptr<VertexBuffer> vbo;
    std::unique_ptr<IndexBuffer> ibo;
    void setupMesh();
};
#endif // MESH_H