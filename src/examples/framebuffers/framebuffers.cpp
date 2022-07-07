#include "framebuffers.h"

#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char** argv)
{
    std::unique_ptr<FrameBuffersApplication> app = std::make_unique<FrameBuffersApplication>(WIDTH, HEIGHT, "Framebuffers");
    app->Run();
}

FrameBuffersLayer::FrameBuffersLayer(const std::string& name) : Layer(name)
{
}
FrameBuffersLayer::~FrameBuffersLayer()
{
}
void FrameBuffersLayer::Init()
{
    std::string basePathAssets = "../data/";
    sponza = std::make_unique<Model>(basePathAssets + "sponza/sponza.obj");
    light = std::make_unique<Model>(basePathAssets +"cube/cube.obj");
    camera = std::make_unique<Camera>();
    outline_shader = std::make_unique<Shader>(basePathAssets +"shaders/light.vert", basePathAssets +"shaders/outline.frag");
    post_processing_shader =
        std::make_unique<Shader>(basePathAssets +"shaders/postprocessing.vert", basePathAssets +"shaders/postprocessing.frag");
    shader = std::make_unique<Shader>(basePathAssets +"shaders/basic.vert", basePathAssets +"shaders/basic.frag");
    light_shader = std::make_unique<Shader>(basePathAssets +"shaders/light.vert", basePathAssets +"shaders/light.frag");

    //    unsigned int framebuffer;
    //    glGenFramebuffers(1, &framebuffer);
    //    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //
    //    // generate texture
    //    unsigned int textureColorbuffer;
    //    glGenTextures(1, &textureColorbuffer);
    //    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //
    //    // attach it to currently bound framebuffer object
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    //    unsigned int rbo;
    //    glGenRenderbuffers(1, &rbo);
    //    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
    //    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    //    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
void FrameBuffersLayer::DeInit()
{
}
void FrameBuffersLayer::Update(float delta_time)
{
    //glStencilMask(0x00);


    //        lightShader->Bind();
    //        lightShader->SetUniformMat4("projection", camera->GetProjectionMatrix());
    //        lightShader->SetUniformMat4("view", camera->GetViewMatrix());



    //glStencilFunc(GL_ALWAYS, 1, 0xFF);
    //glStencilMask(0xFF);


    // Scaled(outline)
    //        outlineShader->Bind();
    //        outlineShader->SetUniformMat4("projection", camera->GetProjectionMatrix());
    //        outlineShader->SetUniformMat4("view", camera->GetViewMatrix());

    //        glm::mat4 lightOutlineTransform = glm::mat4(1.0f);
    //        lightOutlineTransform = glm::translate(lightOutlineTransform, lightPos);
    //        lightOutlineTransform = glm::scale(lightOutlineTransform, glm::vec3(0.24f));
    //        outlineShader->SetUniformMat4("model", lightOutlineTransform);

    //        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //        glStencilMask(0x00);
    //        glDisable(GL_DEPTH_TEST);
    //
    //        light->Draw(*outlineShader);
    //        glStencilMask(0xFF);
    //        glStencilFunc(GL_ALWAYS, 1, 0xFF);
    //        glEnable(GL_DEPTH_TEST);

    //        // Second Pass
    //        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //        glClear(GL_COLOR_BUFFER_BIT);

    //postprocessing->Bind();
    //vao->Bind();
    //        glDisable(GL_DEPTH_TEST);
    //        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    //        glDrawArrays(GL_TRIANGLES, 0, 6);
    //        glEnable(GL_DEPTH_TEST);

}
