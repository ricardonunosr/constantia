#include "opengl_renderer.h"
#include <glad/gl.h>
#include <iostream>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void* read_entire_file(const char* file_path)
{
    void* result;
    FILE* file_handle = fopen(file_path, "r");
    if (file_handle == 0)
    {
        spdlog::error("File doesn't exist.", file_path);
        exit(-1);
    }
    fseek(file_handle, 0L, SEEK_END);
    long file_size = ftell(file_handle);
    fseek(file_handle, 0L, SEEK_SET);
    result = malloc(file_size);
    size_t bytes_read = fread(result, sizeof(char), file_size, file_handle);
    if (bytes_read != (long unsigned int)file_size)
    {
        spdlog::error("Failed to read file {}", file_path);
        exit(-1);
    }
    fclose(file_handle);
    ((char*)result)[file_size] = 0;

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
        spdlog::error("Failed to load texture({}) reason: {}", path, stbi_failure_reason());
    }
    stbi_image_free(data);
}

void opengl_bind_texture(unsigned int id, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void opengl_unbind_textrure()
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

VertexArray::VertexArray() : m_id{0}, m_enabled_attribs{0}
{
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_id);
}

void VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

void VertexArray::add_buffer(VertexBuffer& buffer)
{
    bind();
    buffer.bind();
    const auto& layout = buffer.get_layout().get_elements();
    for (const auto& element : layout)
    {
        glVertexAttribPointer(m_enabled_attribs, (GLint)element.get_component_count(),
                              shader_data_type_to_open_gl_base_type(element.type), (GLint)element.normalized,
                              (GLint)buffer.get_layout().get_stride(), (const void*)element.offset);
        glEnableVertexAttribArray(m_enabled_attribs);
        m_enabled_attribs++;
    }
}

VertexBuffer::VertexBuffer(VertexBufferLayout layout, const void* data, size_t size) : m_layout{std::move(layout)}
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

IndexBuffer::IndexBuffer(const void* indices, unsigned int count) : m_id{0}, m_count{count}
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void IndexBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
