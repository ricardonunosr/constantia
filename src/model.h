#pragma once

#include "opengl_renderer.h"
#include "memory.h"
#include "idk_math.h"
#include <string>
#include <vector>

struct Vertex
{
    idk_vec3 position;
    idk_vec3 normal;
    idk_vec2 tex_coords;

    bool operator==(const Vertex& other) const
    {
        return position == other.position && tex_coords == other.tex_coords;
    }
};

struct Material
{
    Texture* diffuse_tex;
    Texture* specular_tex;
};

struct Model
{
    struct MeshMaterialGroup
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material materials;

        VertexArray* vao;
        VertexBuffer* vbo;
        IndexBuffer* ibo;
    };

    std::vector<MeshMaterialGroup> meshes;
};

Model* create_model(Arena* arena, const std::string& path);
void draw(Model* model, const idk_mat4& transform, OpenGLProgramCommon* shader);

