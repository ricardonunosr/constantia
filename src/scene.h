#include "entity.h"
#include <vector>

class Scene
{
  public:
    Scene();
    ~Scene();

  private:
    std::vector<Entity> entities;
};