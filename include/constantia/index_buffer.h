#pragma once

class IndexBuffer
{
  public:
    IndexBuffer(const void* indices, unsigned int count);
    ~IndexBuffer();

    void Bind() const;
    static void Unbind();

  private:
    unsigned int id;
    unsigned int count;
};