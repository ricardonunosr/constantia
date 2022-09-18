#pragma once

#include <string>
#include <utility>

class Layer
{
  public:
    explicit Layer(std::string name) : name(std::move(name))
    {
    }
    ~Layer() = default;

    virtual void Init()
    {
    }
    virtual void DeInit()
    {
    }
    virtual void Update(float delta_time)
    {
    }

  private:
    std::string name;
};
