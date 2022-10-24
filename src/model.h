#pragma once

#include "opengl_renderer.h"
#include "memory.h"
#include "idk_math.h"

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

struct MeshMaterialGroup
{
  Vertex* vertices;
  uint64_t num_vertices;
  uint32_t* indices;
  uint64_t num_indices;
  Material materials;

  VertexArray* vao;
  VertexBuffer* vbo;
  IndexBuffer* ibo;
};

struct MeshNode
{
  MeshMaterialGroup* data;
  MeshNode* next;
};

struct Model
{
  MeshNode* meshes; // Head of linked list
};

Model* create_model(Arena* arena, const std::string& path);
void draw(Model* model, const idk_mat4& transform, OpenGLProgramCommon* shader);

