#include "vertex_buffer_layout.h"

class VertexBuffer
{
  public:
    VertexBuffer(const VertexBufferLayout& layout, const void* data,unsigned int size);
    ~VertexBuffer();

    void Bind();
    void Unbind();

    VertexBufferLayout GetLayout() const
    {
        return layout;
    }

  private:
    unsigned int id;
    const VertexBufferLayout& layout;
};