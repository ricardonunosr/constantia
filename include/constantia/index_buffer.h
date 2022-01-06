class IndexBuffer
{
  public:
    IndexBuffer(const void* indices, unsigned int count);
    ~IndexBuffer();

    void Bind();
    void Unbind();

  private:
    unsigned int id;
    unsigned int count;
};