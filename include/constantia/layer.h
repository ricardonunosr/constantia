#pragma once

#include <string>

class Layer{
  public:
    explicit Layer(const std::string& name) : name(name)
    {
    }
    ~Layer()= default;

    virtual void Init(){}
    virtual void DeInit(){}
    virtual void Update(float delta_time){}
  private:
    std::string name;
};

