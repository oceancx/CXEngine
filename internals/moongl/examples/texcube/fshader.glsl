#version 330 core

in vec2 TexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{
    gl_FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), 0.5);
}
