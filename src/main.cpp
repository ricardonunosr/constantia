#include "application.h"

#define WIDTH 1920
#define HEIGHT 1080

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<Application> app = std::make_unique<Application>(WIDTH, HEIGHT, "Constantia");
    app->run();
    return 0;
}