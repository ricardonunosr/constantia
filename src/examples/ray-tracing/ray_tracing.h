#pragma once

#include "layer.h"
#include <string>

class RayTracingLayer : public Layer
{
  public:
    explicit RayTracingLayer(const std::string& name);
    ~RayTracingLayer();

    void Init() override;
    void DeInit() override;
    void Update(float delta_time) override;

  private:
};