#pragma once

class IndexBuffer
{
  public:
    IndexBuffer(const void* indices, unsigned int count);
    ~IndexBuffer();

    void bind() const;
    static void unbind();

  private:
    unsigned int m_id;
    unsigned int m_count;
};