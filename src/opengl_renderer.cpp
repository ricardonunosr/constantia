#include "opengl_renderer.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void* read_entire_file(const char* file_path)
{
    void* result;
    FILE* file_handle = fopen(file_path, "r");
    if (file_handle == 0)
    {
        printf("File doesn't exist: %s", file_path);
        exit(-1);
    }
    fseek(file_handle, 0L, SEEK_END);
    long file_size = ftell(file_handle);
    fseek(file_handle, 0L, SEEK_SET);

    result = malloc(file_size);
    fread(result, 1, file_size, file_handle);
    fclose(file_handle);

    return result;
}

void opengl_create_shader(char* vertex_shader_source, char* fragment_shader_source, OpenGLProgramCommon* result)
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
        std::cout << "Vertex errors: " << vertex_errors << "\n";
	std::cout << "Fragment errors: " << fragment_errors << "\n";
        std::cout << "Program errors: " << program_errors << "\n";
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    result->program_id = program_id;
    result->model = glGetUniformLocation(program_id, "model");
    result->projection = glGetUniformLocation(program_id, "projection");
    result->view = glGetUniformLocation(program_id, "view");
    result->material_texture_diffuse = glGetUniformLocation(program_id, "material.texture_diffuse");
    result->material_texture_specular = glGetUniformLocation(program_id, "material.texture_specular");
    result->material_shininess = glGetUniformLocation(program_id, "material.shininess");
}

void opengl_create_texture(const char* path, texture_type type, Texture* texture)
{
    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load(path, &texture->width, &texture->height, &texture->nr_channels, 0);
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
        printf("Failed to load texture(%s) reason: %s", path, stbi_failure_reason());
    }
    stbi_image_free(data);
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

void opengl_create_vertex_array(VertexArray* vertex_array)
{
    glGenVertexArrays(1, &vertex_array->id);
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

void opengl_create_vertex_buffer(const void* data, size_t size, VertexBuffer* vertex_buffer)
{
    glGenBuffers(1, &vertex_buffer->id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void opengl_create_index_buffer(const void* indices, unsigned int count, IndexBuffer* index_buffer)
{
    glGenBuffers(1, &index_buffer->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
