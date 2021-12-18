#version 410

in vec3 FrontColor;
in vec3 BackColor;
layout(location = 0) out vec4 FragColor;

//#define SHOW_BACK_FACES //uncomment to see back faces rendered in red

void main()
    {
    if(gl_FrontFacing)
        FragColor = vec4(FrontColor, 1.0);
    else
#if defined(SHOW_BACK_FACES)
        FragColor = mix(vec4(BackColor, 1.0), vec4(1.0,0.0,0.0,1.0), 0.7);
#else
        FragColor = vec4(BackColor, 1.0);
#endif
    }
