#include "vertex_array.h"
#include "core.h"

static GLenum ShaderDataTypeToOpenGLBaseType(DataType type)
{
    switch (type)
    {
    case DataType::Float:
        return GL_FLOAT;
    case DataType::Float2:
        return GL_FLOAT;
    case DataType::Float3:
        return GL_FLOAT;
    case DataType::Float4:
        return GL_FLOAT;
    case DataType::Mat3:
        return GL_FLOAT;
    case DataType::Mat4:
        return GL_FLOAT;
    case DataType::Int:
        return GL_INT;
    case DataType::Int2:
        return GL_INT;
    case DataType::Int3:
        return GL_INT;
    case DataType::Int4:
        return GL_INT;
    case DataType::Bool:
        return GL_BOOL;
    }

    return 0;
}

VertexArray::VertexArray() : id{0}, enabledAttribs{0}
{
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
}

void VertexArray::Bind()
{
    glBindVertexArray(id);
}

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

void VertexArray::AddBuffer(VertexBuffer& buffer)
{
    Bind();
    buffer.Bind();
    auto& layout = buffer.GetLayout().GetElements();
    for (size_t i = 0; i < layout.size(); i++)
    {
        const auto& element = layout[i];
        glVertexAttribPointer(enabledAttribs, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.type),
                              element.normalized, buffer.GetLayout().GetStride(), (const void*)element.offset);
        glEnableVertexAttribArray(enabledAttribs);
        enabledAttribs++;
    }
}
