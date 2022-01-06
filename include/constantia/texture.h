#include <string>

class Texture
{
  public:
    Texture(const char* path);
    ~Texture();

    void Bind(unsigned int slot = 0);
    void Unbind();

    std::string GetPath();

  private:
    unsigned int id;
    int width;
    int height;
    int nrChannels;
    std::string path;
};
