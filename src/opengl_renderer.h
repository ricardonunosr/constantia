#pragma once
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <utility>
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

void* read_entire_file(const char* file_path);
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
};

void opengl_create_texture(const char* path, texture_type type,Texture *texture);
void opengl_bind_texture(unsigned int id, unsigned int slot);
void opengl_unbind_textrure();

enum class DataType
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

struct VertexBufferElement
{
    const std::string name;
    DataType type;
    uint32_t size{};
    bool normalized{};
    size_t offset{};

    VertexBufferElement() = default;

    VertexBufferElement(std::string name, DataType type, bool normalized = false)
        : name{std::move(name)}, type{type}, size(data_type_size(type)), normalized{normalized}, offset{0}
    {
    }

    [[nodiscard]] uint32_t get_component_count() const
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
};

class VertexBufferLayout
{
  public:
    VertexBufferLayout(std::initializer_list<VertexBufferElement> elements) : m_elements{elements}
    {
        calculate_offset_and_stride();
    };

    [[nodiscard]] const std::vector<VertexBufferElement>& get_elements() const
    {
        return m_elements;
    }

    [[nodiscard]] unsigned int get_stride() const
    {
        return m_stride;
    }

  private:
    std::vector<VertexBufferElement> m_elements;
    unsigned int m_stride{};

    void calculate_offset_and_stride()
    {
        size_t offset = 0;
        m_stride = 0;
        for (auto& element : m_elements)
        {
            m_stride += element.size;
            element.offset = offset;
            offset += element.size;
        }
    }
};

class VertexBuffer
{
  public:
    VertexBuffer(VertexBufferLayout layout, const void* data, size_t size);
    ~VertexBuffer();

    void bind() const;
    static void unbind();

    [[nodiscard]] const VertexBufferLayout& get_layout() const
    {
        return m_layout;
    }

  private:
    unsigned int m_id{};
    VertexBufferLayout m_layout;
};

class VertexArray
{
  public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    static void unbind();

    void add_buffer(VertexBuffer& buffer);

  private:
    unsigned int m_id;
    unsigned int m_enabled_attribs;
};

class IndexBuffer
{
  public:
    IndexBuffer(const void* indices, unsigned int count);
    ~IndexBuffer();

    void bind() const;
    static void unbind();

  private:
    unsigned int m_id;
    unsigned int m_count;
};