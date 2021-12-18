#version 400

layout(vertices=4) out;

uniform int NumSegments;
uniform int NumStrips;

void main()
    {
    // Pass along the vertex position unmodified
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    // Some drivers (e.g. Intel) treat these levels incorrectly.  The OpenGL spec
    // says that level 0 should be the number of strips and level 1 should be
    // the number of segments per strip.  Unfortunately, not all drivers do this.
    // If this example doesn't work for you, try switching the right
    // hand side of the two assignments below.
    gl_TessLevelOuter[0] = float(NumStrips);
    gl_TessLevelOuter[1] = float(NumSegments);
    }
