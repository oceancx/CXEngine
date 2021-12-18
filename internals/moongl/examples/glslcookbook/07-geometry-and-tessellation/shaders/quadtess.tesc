#version 400

layout(vertices=4) out;

uniform int Outer;
uniform int Inner;

void main()
    {
    // Pass along the vertex position unmodified
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    gl_TessLevelOuter[0] = float(Outer);
    gl_TessLevelOuter[1] = float(Outer);
    gl_TessLevelOuter[2] = float(Outer);
    gl_TessLevelOuter[3] = float(Outer);
    gl_TessLevelInner[0] = float(Inner);
    gl_TessLevelInner[1] = float(Inner);
    }
