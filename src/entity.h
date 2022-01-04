#include <glm/glm.hpp>
#include <string>

class Entity
{
  public:
    Entity();
    ~Entity();

  private:
    std::string name;
    glm::mat4 transform;
};