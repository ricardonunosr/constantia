#pragma once

#include "application.h"
#include "model.h"
#include "shader.h"
#include "camera.h"

class FrameBuffersLayer : public Layer
{
  public:
    FrameBuffersLayer(const std::string& name);
    ~FrameBuffersLayer();

    void Init() override;
    void DeInit() override;
    void Update(float delta_time) override;

  private:
    std::unique_ptr<Model> light;
    std::unique_ptr<Model> sponza;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> outline_shader;
    std::unique_ptr<Shader> post_processing_shader;
    std::unique_ptr<Shader> light_shader;
};

class FrameBuffersApplication: public Application
{
  public:
    FrameBuffersApplication(int width, int height, const std::string& name) : Application(width,height,name)
    {
        layers.push_back(new FrameBuffersLayer(name));
    }
};