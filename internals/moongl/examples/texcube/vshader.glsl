#version 330 core

layout(location=0) in vec4 vPosition;
layout(location=1) in vec2 vTexCoords;

out vec2 TexCoord;

uniform vec3 theta;

void main() 
{
    vec3 angles = radians( theta );
    vec3 c = cos( angles );
    vec3 s = sin( angles );

    mat4 rx = mat4( 1.0,  0.0,  0.0, 0.0,
            0.0,  c.x,  s.x, 0.0,
            0.0, -s.x,  c.x, 0.0,
            0.0,  0.0,  0.0, 1.0 );

    mat4 ry = mat4( c.y, 0.0, -s.y, 0.0,
            0.0, 1.0,  0.0, 0.0,
            s.y, 0.0,  c.y, 0.0,
            0.0, 0.0,  0.0, 1.0 );

    mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
            s.z,  c.z, 0.0, 0.0,
            0.0,  0.0, 1.0, 0.0,
            0.0,  0.0, 0.0, 1.0 );

    gl_Position = rz * ry * rx * vPosition;
    //TexCoord = vec2(vTexCoords.x, 1.0 - vTexCoords.y);
    TexCoord = vTexCoords;
} 
