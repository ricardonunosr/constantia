#pragma once

#include "layer.h"
#include <string>

struct Model;
struct Shader;
struct VertexArray;

class FrameBuffersLayer : public Layer
{
  public:
    explicit FrameBuffersLayer(const std::string& name);
    ~FrameBuffersLayer();

    void init() override;
    void de_init() override;
    void update(float delta_time) override;

  private:
    std::unique_ptr<Model> m_light;
    std::unique_ptr<Model> m_cube;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Shader> m_outline_shader;
    std::unique_ptr<Shader> m_post_processing_shader;
    std::unique_ptr<Shader> m_light_shader;
    std::unique_ptr<VertexArray> m_vao;
    unsigned int m_framebuffer{}, m_texture_colorbuffer{};
};