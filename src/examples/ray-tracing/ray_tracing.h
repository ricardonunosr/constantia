#pragma once

#include "layer.h"
#include <string>

class RayTracingLayer : public Layer
{
  public:
    explicit RayTracingLayer(const std::string& name);

    void update(float delta_time) const override;

  private:
};