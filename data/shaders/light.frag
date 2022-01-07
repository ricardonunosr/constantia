#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Image;

void main()
{
    FragColor = texture(Image, TexCoord)* vec4(1.0); // set all 4 vector values to 1.0
}