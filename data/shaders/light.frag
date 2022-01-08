#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

struct Material{
  sampler2D texture_diffuse1;
};

uniform Material material;

void main()
{
    FragColor = texture(material.texture_diffuse1, TexCoord)* vec4(1.0); // set all 4 vector values to 1.0
}