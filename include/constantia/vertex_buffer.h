#include "vertex_buffer_layout.h"

class VertexBuffer
{
  public:
    VertexBuffer(const VertexBufferLayout& layout, const void* data, size_t size);
    ~VertexBuffer();

    void Bind();
    void Unbind();

    const VertexBufferLayout& GetLayout() const
    {
        return layout;
    }

  private:
    unsigned int id;
    VertexBufferLayout layout;
};