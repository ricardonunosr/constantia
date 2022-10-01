#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Texture
{
  public:
    Texture(const char* path, const char* type);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    static void unbind();

    const std::string& get_path()
    {
        return m_path;
    }

    const std::string& get_type()
    {
        return m_type;
    }

    void set_path(const std::string& new_path)
    {
        m_path = new_path;
    }

    void set_type(const std::string& type_name)
    {
        m_type = type_name;
    }

  private:
    unsigned int m_id;
    int m_width;
    int m_height;
    int m_nr_channels;
    std::string m_path;
    std::string m_type;
};

class Shader
{
  public:
    Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
    ~Shader();
    void bind() const;
    static void unbind();

    void set_uniform1i(const std::string& name, int value);
    void set_uniform1f(const std::string& name, float value);
    void set_uniform3f(const std::string& name, float v0, float v1, float v2);
    void set_uniform3f(const std::string& name, const glm::vec3& value);
    void set_uniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void set_uniform_mat4(const std::string& name, const glm::mat4& matrix);

  private:
    unsigned int m_id{};
    std::unordered_map<std::string, int> m_uniform_cache;

    static std::string read_shader_source_from_file(const std::string& shader_path);
    static unsigned int compile_shader(unsigned int type, const char* source);
    void link_program(unsigned int vertex_shader, unsigned int fragment_shader);
    void cache_uniforms();
    int get_location_from_cache(const std::string& name);
};

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