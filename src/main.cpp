#include "application.h"

#define WIDTH 1280
#define HEIGHT 720

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<Application> app = std::make_unique<Application>(WIDTH, HEIGHT, "Application");
    app->run();
    return 0;
}