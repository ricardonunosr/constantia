#pragma once

#include "layer.h"
#include <string>

struct Model;
struct Shader;

class SponzaLayer : public Layer
{
  public:
    explicit SponzaLayer(const std::string& name);
    ~SponzaLayer();

    void Init() override;
    void DeInit() override;
    void Update(float delta_time) override;
    void OnUIRender() override;

  private:
    std::unique_ptr<Model> light;
    std::unique_ptr<Model> sponza;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> light_shader;
};