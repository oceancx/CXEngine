#version 330 core
in vec2 ourTextCoord;
in float ourColor;

out vec4 color;


uniform sampler2D textureSampler;

void main()
{
   vec4 sampledColor = texture(textureSampler, ourTextCoord);
   color = sampledColor *ourColor;

}