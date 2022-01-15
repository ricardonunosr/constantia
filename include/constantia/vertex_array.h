#pragma once

#include "vertex_buffer.h"

class VertexArray
{
  public:
    VertexArray();
    ~VertexArray();

    void Bind();
    void Unbind();

    void AddBuffer(VertexBuffer& buffer);

  private:
    unsigned int id;
    unsigned int enabledAttribs;
};