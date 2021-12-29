#include <string>

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
  std::string ReadShaderSourceFromFile(const std::string& shaderPath);
  unsigned int CompileShader(unsigned int type, const char* source);
};