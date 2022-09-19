#include "vertex_array.h"
#include "core.h"

static GLenum shader_data_type_to_open_gl_base_type(DataType type)
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
        glVertexAttribPointer(m_enabled_attribs, element.get_component_count(),
                              shader_data_type_to_open_gl_base_type(element.type), element.normalized,
                              buffer.get_layout().get_stride(), (const void*)element.offset);
        glEnableVertexAttribArray(m_enabled_attribs);
        m_enabled_attribs++;
    }
}
