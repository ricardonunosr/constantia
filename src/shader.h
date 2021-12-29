#include <string>
#include <unordered_map>

#include "core.h"

class Shader {
 public:
  Shader(const std::string& vertexShaderPath,
         const std::string& fragmentShaderPath);
  void Bind();
  void Unbind();

  void SetUniform4f(const std::string& name, float v0, float v1, float v2,
                    float v3);

 private:
  unsigned int id;

  std::unordered_map<std::string, int> uniform_cache;

  std::string ReadShaderSourceFromFile(const std::string& shaderPath);
  unsigned int CompileShader(unsigned int type, const char* source);
  void LinkProgram(unsigned int vertexShader, unsigned int fragmentShader);
  void CacheUniforms();
};