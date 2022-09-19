#pragma once

#include "vertex_buffer.h"

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