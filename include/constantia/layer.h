#pragma once

#include <string>
#include <utility>

class Layer
{
  public:
    explicit Layer(std::string name) : m_name(std::move(name))
    {
    }
    ~Layer() = default;

    virtual void init()
    {
    }
    virtual void de_init()
    {
    }
    virtual void update(float delta_time)
    {
    }
    virtual void on_ui_render(float delta_time)
    {
    }

  private:
    std::string m_name;
};
