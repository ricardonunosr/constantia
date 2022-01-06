#include "renderer.h"
#include "core.h"

void Renderer::SetClearColor(float v0, float v1, float v2, float v3)
{
    glClearColor(v0, v1, v2, v3);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
