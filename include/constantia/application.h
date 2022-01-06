#include "core.h"
#include "window.h"

class Application
{
  public:
    Application(int width, int height, const std::string& name);
    ~Application();

    void Init();
    void Update();
    void Cleanup();

  private:
    static Application* instance;
    std::unique_ptr<Window> window;
    float deltaTime = 0, lastFrame = 0;
};