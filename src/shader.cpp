#include "shader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Shader::Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
{
    const std::string vertex_shader_source = read_shader_source_from_file(vertex_shader_path);
    const std::string fragment_shader_source = read_shader_source_from_file(fragment_shader_path);

    unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source.c_str());
    unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source.c_str());

    link_program(vertex_shader, fragment_shader);
    cache_uniforms();
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::bind() const
{
    glUseProgram(m_id);
}

void Shader::unbind()
{
    glUseProgram(0);
}

void Shader::set_uniform1i(const std::string& name, int value)
{
    int location = get_location_from_cache(name);
    glUniform1i(location, value);
}

void Shader::set_uniform1f(const std::string& name, float value)
{
    int location = get_location_from_cache(name);
    glUniform1f(location, value);
}

void Shader::set_uniform3f(const std::string& name, float v0, float v1, float v2)
{
    int location = get_location_from_cache(name);
    glUniform3f(location, v0, v1, v2);
}

void Shader::set_uniform3f(const std::string& name, const glm::vec3& value)
{
    int location = get_location_from_cache(name);
    glUniform3fv(location, 1, &value[0]);
}

void Shader::set_uniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    int location = get_location_from_cache(name);
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::set_uniform_mat4(const std::string& name, const glm::mat4& matrix)
{
    int location = get_location_from_cache(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::string Shader::read_shader_source_from_file(const std::string& shader_path)
{
    std::string result;
    std::ifstream filestream;

    std::string line;
    filestream.open(shader_path);
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
        spdlog::error("Failed to read {}! File doesn't exist.", shader_path);
    }

    return result;
}

unsigned int Shader::compile_shader(unsigned int type, const char* source)
{
    unsigned int shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check if shader was compiled successfully
    int success;
    int length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)malloc(length * sizeof(char));

    if (success == 0)
    {
        glGetShaderInfoLog(shader, 512, &length, message);
        spdlog::error("Failed to compile {} shader! {}", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), message);
        glDeleteShader(shader);
        free(message);
        return 0;
    }
    free(message);

    return shader;
}

void Shader::link_program(unsigned int vertex_shader, unsigned int fragment_shader)
{
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex_shader);
    glAttachShader(m_id, fragment_shader);
    glLinkProgram(m_id);

    int success;
    int length;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)malloc(length * sizeof(char));

    if (success == 0)
    {
        glGetProgramInfoLog(m_id, 512, &length, message);
        spdlog::error("Failed to link shaders! {}", message);
        free(message);
        return;
    }

    glUseProgram(m_id);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    free(message);
}

void Shader::cache_uniforms()
{
    int longest_uniform_max_length_name;
    int num_uniforms;
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &longest_uniform_max_length_name);
    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &num_uniforms);

    char* name = (char*)malloc(longest_uniform_max_length_name * sizeof(char));
    for (size_t i = 0; i < num_uniforms; i++)
    {
        int size;
        unsigned int type;
        glGetActiveUniform(m_id, i, longest_uniform_max_length_name, NULL, &size, &type, name);
        int location = glGetUniformLocation(m_id, name);
        m_uniform_cache[std::string(name)] = location;
    }
    free(name);
}

int Shader::get_location_from_cache(const std::string& name)
{
    auto location_iter = m_uniform_cache.find(name);
    if (location_iter != m_uniform_cache.end())
    {
        auto location = location_iter->second;
        return location;
    }

    spdlog::error("Couldn't find cached uniform:{} in shader with id {}", name, m_id);
    return 0;
}
