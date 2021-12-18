#version 400

layout(vertices=16) out;

uniform int MinTessLevel;
uniform int MaxTessLevel;
uniform float MaxDepth;
uniform float MinDepth;
uniform mat4 ModelViewMatrix;

void main()
    {
    // Position in camera coordinates
    vec4 p = ModelViewMatrix * gl_in[gl_InvocationID].gl_Position;
    // Distance from camera scaled between 0 and 1
    float depth = clamp((abs(p.z) - MinDepth) / (MaxDepth - MinDepth), 0.0, 1.0);
    float tessLevel = mix(MaxTessLevel, MinTessLevel, depth);
    gl_TessLevelOuter[0] = tessLevel;
    gl_TessLevelOuter[1] = tessLevel;
    gl_TessLevelOuter[2] = tessLevel;
    gl_TessLevelOuter[3] = tessLevel;
    gl_TessLevelInner[0] = tessLevel;
    gl_TessLevelInner[1] = tessLevel;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    }
