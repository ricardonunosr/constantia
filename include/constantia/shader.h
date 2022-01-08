#include <string>
#include <unordered_map>

#include "core.h"
#include <glm/glm.hpp>

class Shader
{
  public:
    Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void Bind();
    void Unbind();

    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform3f(const std::string& name, const glm::vec3& value);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4(const std::string& name, const glm::mat4& matrix);

  private:
    unsigned int id;

    std::unordered_map<std::string, int> uniform_cache;

    std::string ReadShaderSourceFromFile(const std::string& shaderPath);
    unsigned int CompileShader(unsigned int type, const char* source);
    void LinkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    void CacheUniforms();
    int GetLocationFromCache(const std::string& name);
};