#pragma once

#include "vertex_buffer_layout.h"

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