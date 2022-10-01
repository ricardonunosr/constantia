#pragma once

#include <string>
#include <utility>

class Layer
{
  public:
    explicit Layer(std::string name) : m_name(std::move(name))
    {
    }
    virtual void update(float delta_time) const = 0;
    virtual void on_ui_render(float delta_time) const = 0;

  private:
    std::string m_name;
};
