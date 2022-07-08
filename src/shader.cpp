#include "shader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    const std::string vertexShaderSource = ReadShaderSourceFromFile(vertexShaderPath);
    const std::string fragmentShaderSource = ReadShaderSourceFromFile(fragmentShaderPath);

    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

    LinkProgram(vertexShader, fragmentShader);
    CacheUniforms();
}

void Shader::Bind()
{
    glUseProgram(id);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    int location = GetLocationFromCache(name);
    glUniform1i(location, value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    int location = GetLocationFromCache(name);
    glUniform1f(location, value);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    int location = GetLocationFromCache(name);
    glUniform3f(location, v0, v1, v2);
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& value)
{
    int location = GetLocationFromCache(name);
    glUniform3fv(location, 1, &value[0]);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    int location = GetLocationFromCache(name);
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    int location = GetLocationFromCache(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::string Shader::ReadShaderSourceFromFile(const std::string& shaderPath)
{
    std::string result;
    std::ifstream filestream;

    std::string line = "";
    filestream.open(shaderPath);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            result.append(line + "\n");
        }
        filestream.close();
    }
    else
    {
        spdlog::error("Failed to read {}! File doesn't exist.", shaderPath);
    }

    return result;
}

unsigned int Shader::CompileShader(unsigned int type, const char* source)
{
    unsigned int shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check if shader was compiled successufully
    int success;
    int length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)malloc(length * sizeof(char));

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, &length, message);
        spdlog::error("Failed to compile {} shader! {}", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), message);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void Shader::LinkProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    int success;
    int length;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)malloc(length * sizeof(char));

    if (!success)
    {
        glGetProgramInfoLog(id, 512, &length, message);
        spdlog::error("Failed to link shaders! {}", message);
    }

    glUseProgram(id);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::CacheUniforms()
{
    int longestUniformMaxLengthName;
    int numUniforms;
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &longestUniformMaxLengthName);
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numUniforms);

    char* name = (char*)malloc(longestUniformMaxLengthName * sizeof(char));
    for (size_t i = 0; i < numUniforms; i++)
    {
        int size;
        unsigned int type;
        glGetActiveUniform(id, i, longestUniformMaxLengthName, NULL, &size, &type, name);
        int location = glGetUniformLocation(id, name);
        uniform_cache[std::string(name)] = location;
    }
}

int Shader::GetLocationFromCache(const std::string& name)
{
    auto location_iter = uniform_cache.find(name);
    if (location_iter != uniform_cache.end())
    {
        auto location = location_iter->second;
        return location;
    }
    else
    {
        spdlog::error("Couldn't find cached uniform:{} in shader with id {}", name, id);
    }

    return 0;
}
