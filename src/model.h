#pragma once

#include "opengl_renderer.h"
#include "idk_math.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex
{
    // TODO(ricardo): change this to our own math library. Need to implement hash function.
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;

    bool operator==(const Vertex& other) const
    {
        return position == other.position && tex_coords == other.tex_coords;
    }
};

struct Model
{
    struct MeshMaterialGroup
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture*> textures;

        VertexArray* vao;
        VertexBuffer* vbo;
        IndexBuffer* ibo;
    };

    std::vector<MeshMaterialGroup> meshes;
};

void create_model(Model* model, const std::string& path);
void draw(Model* model, const idk_mat4& transform, OpenGLProgramCommon* shader);

