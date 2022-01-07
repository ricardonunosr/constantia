#version 410 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Image;

void main()
{
	FragColor = texture(Image, TexCoord);
}