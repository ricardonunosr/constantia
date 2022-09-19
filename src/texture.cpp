#include "texture.h"

#include "core.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const char* path, const char* type)
    : m_id{0}, m_width{0}, m_height{0}, m_nr_channels{0}, m_path{path}, m_type{type}
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load(path, &m_width, &m_height, &m_nr_channels, 0);
    if (data != nullptr)
    {
        unsigned int format = GL_RGB;
        if (m_nr_channels == 1)
            format = GL_RED;
        else if (m_nr_channels == 3)
            format = GL_RGB;
        else if (m_nr_channels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        spdlog::error("Failed to load texture({}) reason: {}", path, stbi_failure_reason());
    }
    stbi_image_free(data);
}

Texture::~Texture()
{
    // glDeleteTextures(1, &id);
}

void Texture::bind(unsigned int slot /*= 0*/) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
