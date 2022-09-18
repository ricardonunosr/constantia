#pragma once

#include "vertex_buffer_layout.h"

class VertexBuffer
{
  public:
    VertexBuffer(VertexBufferLayout layout, const void* data, size_t size);
    ~VertexBuffer();

    void Bind() const;
    static void Unbind();

    [[nodiscard]] const VertexBufferLayout& GetLayout() const
    {
        return layout;
    }

  private:
    unsigned int id{};
    VertexBufferLayout layout;
};