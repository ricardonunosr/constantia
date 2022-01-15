#include "texture.h"

#include "core.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const char* path) : id{0}, width{0}, height{0}, nrChannels{0}, path{""}, type{"texture_diffuse"}
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        unsigned int format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

void Texture::Bind(unsigned int slot /*= 0*/)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}
void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}