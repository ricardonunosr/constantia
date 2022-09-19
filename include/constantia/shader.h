#pragma once

#include <string>
#include <unordered_map>

#include "core.h"
#include <glm/glm.hpp>

class Shader
{
  public:
    Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
    ~Shader();
    void bind() const;
    static void unbind();

    void set_uniform1i(const std::string& name, int value);
    void set_uniform1f(const std::string& name, float value);
    void set_uniform3f(const std::string& name, float v0, float v1, float v2);
    void set_uniform3f(const std::string& name, const glm::vec3& value);
    void set_uniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void set_uniform_mat4(const std::string& name, const glm::mat4& matrix);

  private:
    unsigned int m_id{};
    std::unordered_map<std::string, int> m_uniform_cache;

    static std::string read_shader_source_from_file(const std::string& shader_path);
    static unsigned int compile_shader(unsigned int type, const char* source);
    void link_program(unsigned int vertex_shader, unsigned int fragment_shader);
    void cache_uniforms();
    int get_location_from_cache(const std::string& name);
};
