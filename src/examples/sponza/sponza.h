#pragma once

#include "application.h"
#include "camera.h"
#include "model.h"

class SponzaLayer : public Layer
{
  public:
    explicit SponzaLayer(const std::string& name);
    ~SponzaLayer();

    void Init() override;
    void DeInit() override;
    void Update(float delta_time) override;

  private:
    std::unique_ptr<Model> light;
    std::unique_ptr<Model> sponza;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> light_shader;
};

class SponzaApplication : public Application
{
  public:
    SponzaApplication(int width, int height, const std::string& name) : Application(width,height,name)
    {
        layers.push_back(new SponzaLayer(name));
    }
};
