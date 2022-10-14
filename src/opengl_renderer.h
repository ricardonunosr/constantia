#pragma once
#include <glad/gl.h>

#include <string>
#include <vector>

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

ReadEntireFile read_entire_file(const char* file_path);
void opengl_create_shader(char* vertex_shader_source, char* fragment_shader_source, OpenGLProgramCommon* result);

enum texture_type
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
    texture_type type;
    std::string name;
};

void opengl_create_texture(const std::string path, texture_type type, Texture* texture);
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

void opengl_create_vertex_buffer(const void* data, size_t size, VertexBuffer* vertex_buffer);

struct VertexArray
{
    unsigned int id;
    unsigned int enabled_attribs;
};

void opengl_create_vertex_array(VertexArray* vertex_array);
void opengl_add_element_to_layout(DataType type, bool normalized, int* enabled_attribs, int stride, int* offset,
                                  VertexArray* vertex_array, VertexBuffer* buffer);

struct IndexBuffer
{
    unsigned int id;
    unsigned int count;
};

void opengl_create_index_buffer(const void* indices, unsigned int count, IndexBuffer* index_buffer);
