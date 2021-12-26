#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
uniform vec3 textColor;
uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(vertex.x,vertex.y,0.0, 1.0);
	TexCoords = vertex.zw;
}
