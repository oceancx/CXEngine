#version 400

layout(vertices=16) out;

uniform int TessLevel;

void main()
    {
    // Pass along the vertex position unmodified
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    gl_TessLevelOuter[0] = float(TessLevel);
    gl_TessLevelOuter[1] = float(TessLevel);
    gl_TessLevelOuter[2] = float(TessLevel);
    gl_TessLevelOuter[3] = float(TessLevel);
    gl_TessLevelInner[1] = float(TessLevel);
    gl_TessLevelInner[0] = float(TessLevel);
    }
