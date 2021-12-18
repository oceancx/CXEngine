#version 330 core
in vec2 ourTextCoord;

out vec4 color;

uniform vec4 solidColor;
uniform sampler2D textureSampler;

void main()
{
	//color = solidColor;
	color = texture(textureSampler, ourTextCoord);
}