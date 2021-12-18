#version 410

/* Use the centroid qualifier to avoid the yellow bleeding on the edges */
// centroid in vec2 TexCoord; 
in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

void main()
    {
    vec3 yellow = vec3(1.0,1.0,0.0);
    vec3 color = vec3(0.0);
    if(TexCoord.s > 1.0) color = yellow;
    FragColor = vec4(color , 1.0);
    }
