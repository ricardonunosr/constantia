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

    void init() override;
    void de_init() override;
    void update(float delta_time) override;
    void on_ui_render() override;

  private:
    std::unique_ptr<Model> m_light;
    std::unique_ptr<Model> m_sponza;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Shader> m_light_shader;
};