#pragma once

#include <string>
#include <utility>
#include <vector>

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