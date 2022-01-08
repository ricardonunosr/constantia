#include <string>

class Texture
{
  public:
    Texture(const char* path);
    ~Texture();

    void Bind(unsigned int slot = 0);
    void Unbind();

    const std::string& GetPath()
    {
        return path;
    }

    const std::string& GetType()
    {
        return type;
    }

    void SetPath(const std::string& newPath)
    {
        path=newPath;
    }

    void SetType(const std::string& typeName)
    {
        type=typeName;
    }


  private:
    unsigned int id;
    int width;
    int height;
    int nrChannels;
    std::string path;
    std::string type;
};
