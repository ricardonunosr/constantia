#pragma once

#include "layer.h"
#include <string>

class RayTracingLayer : public Layer
{
  public:
    explicit RayTracingLayer(const std::string& name);
    ~RayTracingLayer();

    void init() override;
    void de_init() override;
    void update(float delta_time) override;

  private:
};