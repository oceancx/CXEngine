#version 330 core 
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in float colorMultiplier;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float ourColor;
out vec2 ourTextCoord;

void main()
{
	gl_Position = projection * model * view * vec4(position ,  1.0f);
	ourTextCoord = texCoord;
	ourColor = colorMultiplier;
}