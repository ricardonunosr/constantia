#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

struct Material{
  sampler2D texture_diffuse;
  sampler2D texture_specular;
};

uniform Material material;

void main()
{
    FragColor = texture(material.texture_diffuse, TexCoord)*texture(material.texture_specular, TexCoord)* vec4(1.0); // set all 4 vector values to 1.0
}