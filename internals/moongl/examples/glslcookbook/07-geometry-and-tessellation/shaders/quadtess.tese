#version 400

layout(quads, equal_spacing, ccw) in;

uniform mat4 MVP;

void main()
    {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p10 = gl_in[1].gl_Position;
    vec4 p11 = gl_in[2].gl_Position;
    vec4 p01 = gl_in[3].gl_Position;

    // Linear interpolation
    gl_Position =
        p00 * (1.0-u) * (1.0-v) +
        p10 * u * (1.0-v) +
        p01 * v * (1.0-u) +
        p11 * u * v;

    // Transform to clip coordinates
    gl_Position = MVP * gl_Position;
    }
