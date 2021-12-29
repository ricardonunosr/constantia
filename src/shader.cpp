#include "shader.h"

#include <fstream>
#include <iostream>

Shader::Shader(const std::string& vertexShaderPath,
               const std::string& fragmentShaderPath) {
  const std::string vertexShaderSource =
      ReadShaderSourceFromFile(vertexShaderPath);
  const std::string fragmentShaderSource =
      ReadShaderSourceFromFile(fragmentShaderPath);

  unsigned int vertexShader =
      CompileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
  unsigned int fragmentShader =
      CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

  id = glCreateProgram();
  glAttachShader(id, vertexShader);
  glAttachShader(id, fragmentShader);
  glLinkProgram(id);

  int success;
  char infoLog[512];
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 512, NULL, infoLog);
    std::cout << "ERROR LINKING SHADERS: \n" << infoLog << std::endl;
  }

  glUseProgram(id);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::Bind() { glUseProgram(id); }

void Shader::Unbind() { glUseProgram(0); }

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2,
                          float v3) {
  int vertexColorLocation = glGetUniformLocation(id, name.c_str());
  glUniform4f(vertexColorLocation, v0, v1, v2, v3);
}

std::string Shader::ReadShaderSourceFromFile(const std::string& shaderPath) {
  std::string result;
  std::ifstream filestream;

  std::string line = "";
  filestream.open(shaderPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      result.append(line + "\n");
    }
    filestream.close();
  }

  return result;
}

unsigned int Shader::CompileShader(unsigned int type, const char* source) {
  unsigned int shader;
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  // Check if shader was compiled successufully
  int success;
  int length;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
  char* message = (char*)alloca(length * sizeof(char));

  if (!success) {
    glGetShaderInfoLog(shader, 512, &length, message);
    std::cout << "Failed to compile "
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!"
              << std::endl;
    std::cout << message << std::endl;

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}
