#include "opengl_renderer.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

ReadEntireFile read_entire_file(const char* file_path)
{
    ReadEntireFile result = {};
    FILE* file_handle = fopen(file_path, "rb");
    if (file_handle == 0)
    {
        printf("File doesn't exist: %s", file_path);
        return result;
    }
    fseek(file_handle, 0L, SEEK_END);
    result.size = ftell(file_handle);
    fseek(file_handle, 0L, SEEK_SET);

    result.content = (char*)malloc(result.size+1);
    fread(result.content, 1, result.size, file_handle);
    fclose(file_handle);
    result.content[result.size] = '\0';

    return result;
}

void opengl_create_shader(Arena* arena, char* vertex_shader_source, char* fragment_shader_source, OpenGLProgramCommon* program)
{
    GLchar* vertex_shader_code[] = {vertex_shader_source};
    GLuint vertex_shader_id;
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, vertex_shader_code, 0);
    glCompileShader(vertex_shader_id);

    GLchar* fragment_shader_code[] = {fragment_shader_source};
    GLuint fragment_shader_id;
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, fragment_shader_code, 0);
    glCompileShader(fragment_shader_id);

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    glValidateProgram(program_id);
    GLint linked = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        GLsizei ignored;
        char vertex_errors[512];
        char fragment_errors[512];
        char program_errors[512];
        glGetShaderInfoLog(vertex_shader_id, sizeof(vertex_errors), &ignored, vertex_errors);
        glGetShaderInfoLog(fragment_shader_id, sizeof(fragment_errors), &ignored, fragment_errors);
        glGetProgramInfoLog(program_id, sizeof(program_errors), &ignored, program_errors);
        printf("Vertex errors: %s\n", vertex_errors);
	    printf("Fragment errors: %s\n", fragment_errors);
        printf("Program errors: %s\n", program_errors);
    } 

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
    printf("Created shader successfully\n");

    program->program_id = program_id;
    program->model = glGetUniformLocation(program_id, "model");
    program->projection = glGetUniformLocation(program_id, "projection");
    program->view = glGetUniformLocation(program_id, "view");
    program->view_pos = glGetUniformLocation(program_id, "viewPos");
    program->material_texture_diffuse = glGetUniformLocation(program_id, "material.texture_diffuse");
    program->material_texture_specular = glGetUniformLocation(program_id, "material.texture_specular");
    program->material_shininess = glGetUniformLocation(program_id, "material.shininess");
}

Texture* opengl_create_texture(Arena* arena, const std::string path, TextureType type)
{
    Texture* texture = (Texture*)arena_push(arena, sizeof(Texture));
    texture->name = path;
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load(path.c_str(), &texture->width, &texture->height, &texture->nr_channels, 0);
    if (data != nullptr)
    {
        int format = GL_RGB;
        if (texture->nr_channels == 1)
            format = GL_RED;
        else if (texture->nr_channels == 3)
            format = GL_RGB;
        else if (texture->nr_channels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        texture->type = type;
    }
    else
    {
        printf("Failed to load texture(%s) reason: %s", path.c_str(), stbi_failure_reason());
    }
    stbi_image_free(data);
    return texture;
}

void opengl_bind_texture(unsigned int id, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void opengl_unbind_texture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

static GLenum shader_data_type_to_open_gl_base_type(DataType type)
{
    switch (type)
    {
    case DataType::Float:
    case DataType::Float2:
    case DataType::Float3:
    case DataType::Float4:
    case DataType::Mat3:
    case DataType::Mat4:
        return GL_FLOAT;
    case DataType::Int:
    case DataType::Int2:
    case DataType::Int3:
    case DataType::Int4:
        return GL_INT;
    case DataType::Bool:
        return GL_BOOL;
    case DataType::None:
        break;
    }
    return 0;
}

static uint32_t data_type_size(DataType type)
{
    switch (type)
    {
    case DataType::Float:
        return 4;
    case DataType::Float2:
        return 4 * 2;
    case DataType::Float3:
        return 4 * 3;
    case DataType::Float4:
        return 4 * 4;
    case DataType::Mat3:
        return 4 * 3 * 3;
    case DataType::Mat4:
        return 4 * 4 * 4;
    case DataType::Int:
        return 4;
    case DataType::Int2:
        return 4 * 2;
    case DataType::Int3:
        return 4 * 3;
    case DataType::Int4:
        return 4 * 4;
    case DataType::Bool:
        return 1;
    case DataType::None:
        return 0;
    }

    return 0;
}

static uint32_t get_component_count(DataType type)
{
    switch (type)
    {
    case DataType::Float:
        return 1;
    case DataType::Float2:
        return 2;
    case DataType::Float3:
        return 3;
    case DataType::Float4:
        return 4;
    case DataType::Mat3:
        return 3;
    case DataType::Mat4:
        return 4;
    case DataType::Int:
        return 1;
    case DataType::Int2:
        return 2;
    case DataType::Int3:
        return 3;
    case DataType::Int4:
        return 4;
    case DataType::Bool:
        return 1;
    case DataType::None:
        return 0;
    }
    return 0;
}

VertexArray* opengl_create_vertex_array(Arena* arena)
{
    VertexArray* vertex_array = (VertexArray*)arena_push(arena, sizeof(VertexArray));
    glGenVertexArrays(1, &vertex_array->id);
    return vertex_array;
}

void opengl_add_element_to_layout(DataType type, bool normalized, int* enabled_attribs, int stride, int* offset,
                                  VertexArray* vertex_array, VertexBuffer* buffer)
{
    glBindVertexArray(vertex_array->id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
    glVertexAttribPointer(*enabled_attribs, (GLint)get_component_count(type),
                          shader_data_type_to_open_gl_base_type(type), normalized, stride, (const void*)*offset);
    glEnableVertexAttribArray(*enabled_attribs);
    *offset += data_type_size(type);
    (*enabled_attribs)++;
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer* opengl_create_vertex_buffer(Arena* arena, const void* data, size_t size)
{
    VertexBuffer* vertex_buffer = (VertexBuffer*)arena_push(arena, sizeof(VertexBuffer));
    glGenBuffers(1, &vertex_buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vertex_buffer;
}

IndexBuffer* opengl_create_index_buffer(Arena* arena, const void* indices, unsigned int count)
{
    IndexBuffer* index_buffer = (IndexBuffer*)arena_push(arena, sizeof(IndexBuffer));
    glGenBuffers(1, &index_buffer->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return index_buffer;
}
