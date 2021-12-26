#version 330 core
out vec4 color;
uniform vec4 color_in;
void main()
{
	color = color_in;
}