#include "opengl_renderer.h"
#include <glad/gl.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Shader::Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
{
    const std::string vertex_shader_source = read_shader_source_from_file(vertex_shader_path);
    const std::string fragment_shader_source = read_shader_source_from_file(fragment_shader_path);

    unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source.c_str());
    unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source.c_str());

    link_program(vertex_shader, fragment_shader);
    cache_uniforms();
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::bind() const
{
    glUseProgram(m_id);
}

void Shader::unbind()
{
    glUseProgram(0);
}

void Shader::set_uniform1i(const std::string& name, int value)
{
    int location = get_location_from_cache(name);
    glUniform1i(location, value);
}

void Shader::set_uniform1f(const std::string& name, float value)
{
    int location = get_location_from_cache(name);
    glUniform1f(location, value);
}

void Shader::set_uniform3f(const std::string& name, float v0, float v1, float v2)
{
    int location = get_location_from_cache(name);
    glUniform3f(location, v0, v1, v2);
}

void Shader::set_uniform3f(const std::string& name, const glm::vec3& value)
{
    int location = get_location_from_cache(name);
    glUniform3fv(location, 1, &value[0]);
}

void Shader::set_uniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    int location = get_location_from_cache(name);
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::set_uniform_mat4(const std::string& name, const glm::mat4& matrix)
{
    int location = get_location_from_cache(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::string Shader::read_shader_source_from_file(const std::string& shader_path)
{
    std::string result;
    std::ifstream filestream;

    std::string line;
    filestream.open(shader_path);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            result.append(line + "\n");
        }
        filestream.close();
    }
    else
    {
        spdlog::error("Failed to read {}! File doesn't exist.", shader_path);
    }

    return result;
}

unsigned int Shader::compile_shader(unsigned int type, const char* source)
{
    unsigned int shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check if shader was compiled successfully
    int success;
    int length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)malloc(length * sizeof(char));

    if (success == 0)
    {
        glGetShaderInfoLog(shader, 512, &length, message);
        spdlog::error("Failed to compile {} shader! {}", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), message);
        glDeleteShader(shader);
        free(message);
        return 0;
    }
    free(message);

    return shader;
}

void Shader::link_program(unsigned int vertex_shader, unsigned int fragment_shader)
{
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex_shader);
    glAttachShader(m_id, fragment_shader);
    glLinkProgram(m_id);

    int success;
    int length;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)malloc(length * sizeof(char));

    if (success == 0)
    {
        glGetProgramInfoLog(m_id, 512, &length, message);
        spdlog::error("Failed to link shaders! {}", message);
        free(message);
        return;
    }

    glUseProgram(m_id);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    free(message);
}

void Shader::cache_uniforms()
{
    int longest_uniform_max_length_name;
    int num_uniforms;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &longest_uniform_max_length_name);
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &num_uniforms);

    char* name = (char*)malloc(longest_uniform_max_length_name * sizeof(char));
    for (int uniform_index = 0; uniform_index < num_uniforms; uniform_index++)
    {
        int size;
        unsigned int type;
        glGetActiveUniform(m_id, uniform_index, longest_uniform_max_length_name, NULL, &size, &type, name);
        int location = glGetUniformLocation(m_id, name);
        m_uniform_cache[std::string(name)] = location;
    }
    free(name);
}

int Shader::get_location_from_cache(const std::string& name)
{
    auto location_iter = m_uniform_cache.find(name);
    if (location_iter != m_uniform_cache.end())
    {
        auto location = location_iter->second;
        return location;
    }

    spdlog::error("Couldn't find cached uniform:{} in shader with id {}", name, m_id);
    return 0;
}

Texture::Texture(const char* path, const char* type)
    : m_id{0}, m_width{0}, m_height{0}, m_nr_channels{0}, m_path{path}, m_type{type}
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load(path, &m_width, &m_height, &m_nr_channels, 0);
    if (data != nullptr)
    {
        int format = GL_RGB;
        if (m_nr_channels == 1)
            format = GL_RED;
        else if (m_nr_channels == 3)
            format = GL_RGB;
        else if (m_nr_channels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        spdlog::error("Failed to load texture({}) reason: {}", path, stbi_failure_reason());
    }
    stbi_image_free(data);
}

Texture::~Texture()
{
    // glDeleteTextures(1, &id);
}

void Texture::bind(unsigned int slot /*= 0*/) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
void Texture::unbind()
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
