#include "mesh.h"
#include <string>
#include <tiny_obj_loader.h>
#include <vector>

class Model
{
    struct MeshMaterialGroup
    {
        std::vector<Vertex> vertices{};
        std::vector<unsigned int> indices{};

        std::string diffuse_path = "";
        std::string specular_path = "";
    };

  public:
    Model(const std::string& path)
    {
        loadModel(path);
    }

    void Draw(Shader& shader);

  private:
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(const std::string& path);
};