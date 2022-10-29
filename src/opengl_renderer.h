#pragma once
#include <glad/gl.h>
#include "memory.h"

#include <string>

struct OpenGLProgramCommon
{
  GLint program_id;
  GLint model;
  GLint view;
  GLint projection;
  GLuint view_pos;

  GLint material_texture_diffuse;
  GLint material_texture_specular;
  GLint material_shininess;
};

struct ReadEntireFile
{
  char* content;
  uint32_t size;
};

ReadEntireFile read_entire_file(Arena* arena, const char* file_path);
void opengl_create_shader(Arena* arena, char* vertex_shader_source, char* fragment_shader_source, OpenGLProgramCommon* program);

enum TextureType
{
  diffuse,
  specular
};

struct Texture
{
  unsigned int id;
  int width;
  int height;
  int nr_channels;
  TextureType type;
  std::string name;
};

Texture* opengl_create_texture(Arena* arena, const std::string path, TextureType type);
void opengl_bind_texture(unsigned int id, unsigned int slot);
void opengl_unbind_texture();

enum DataType
{
  None = 0,
  Float,
  Float2,
  Float3,
  Float4,
  Mat3,
  Mat4,
  Int,
  Int2,
  Int3,
  Int4,
  Bool
};

struct VertexBuffer
{
  unsigned int id;
};

VertexBuffer* opengl_create_vertex_buffer(Arena* arena, const void* data, size_t size);

struct VertexArray
{
  unsigned int id;
  unsigned int enabled_attribs;
};

VertexArray* opengl_create_vertex_array(Arena* arena);
void opengl_add_element_to_layout(DataType type, bool normalized, int* enabled_attribs, int stride, int* offset,
    VertexArray* vertex_array, VertexBuffer* buffer);

struct IndexBuffer
{
  unsigned int id;
  unsigned int count;
};

IndexBuffer* opengl_create_index_buffer(Arena* arena, const void* indices, unsigned int count);
