#include <string>
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

static uint32_t DataTypeSize(DataType type)
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
    }

    return 0;
};

struct VertexBufferElement
{
    const std::string name;
    DataType type;
    uint32_t size;
    bool normalized;
    size_t offset;

    VertexBufferElement() = default;

    VertexBufferElement(const std::string& name, DataType type, bool normalized = false)
        : name{name}, type{type}, size(DataTypeSize(type)), normalized{normalized}, offset{0}
    {
    }

    uint32_t GetComponentCount() const
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
        }

        return 0;
    }
};

class VertexBufferLayout
{
  public:
    VertexBufferLayout(std::initializer_list<VertexBufferElement> elements) : elements{elements}
    {
        CalculateOffsetAndStride();
    };

    std::vector<VertexBufferElement> GetElements()
    {
        return elements;
    }

    unsigned int GetStride()
    {
        return stride;
    }

  private:
    std::vector<VertexBufferElement> elements;
    unsigned int stride;

    void CalculateOffsetAndStride()
    {
        size_t offset = 0;
        stride = 0;
        for (size_t i = 0; i < elements.size(); i++)
        {
            auto& element = elements[i];
            stride += element.size;
            element.offset = offset;
            offset += element.size;
        }
    }
};