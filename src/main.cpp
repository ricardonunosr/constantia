#include "application.h"

#define WIDTH 1280
#define HEIGHT 720

int main()
{
    Application* app = new Application(WIDTH, HEIGHT, "Constantia");
    app->Update();
    delete app;
}