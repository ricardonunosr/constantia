#include <string>

class Texture
{
  public:
    Texture(const std::string& path, bool transparent = false);
    ~Texture();

    void Bind(unsigned int slot = 0);
    void Unbind();

  private:
    unsigned int id;
    int width;
    int height;
    int nrChannels;
};
