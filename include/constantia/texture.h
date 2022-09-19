#pragma once

#include <string>

class Texture
{
  public:
    Texture(const char* path, const char* type);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    static void unbind();

    const std::string& get_path()
    {
        return m_path;
    }

    const std::string& get_type()
    {
        return m_type;
    }

    void set_path(const std::string& new_path)
    {
        m_path = new_path;
    }

    void set_type(const std::string& type_name)
    {
        m_type = type_name;
    }

  private:
    unsigned int m_id;
    int m_width;
    int m_height;
    int m_nr_channels;
    std::string m_path;
    std::string m_type;
};
