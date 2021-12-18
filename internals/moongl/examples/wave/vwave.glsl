attribute vec4 vPosition;
uniform float time;
varying vec4 color;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;

void main()
{
    vec4 vNormal = vec4(0.0, 1.0, 0.0, 0.0);
    vec4 t = vPosition;
    t.y = 0.1*sin(0.001*time+5.0*vPosition.x)*sin(0.001*time+5.0*vPosition.z);
    gl_Position = ModelViewProjectionMatrix * t;

    vec4 ambient_product = vec4( 0.0, 0.0, 0.0, 1.0);
     vec4 diffuse_product = vec4( 0.3, 0.5, 0.4, 1.0);
     vec4 specular_product = vec4( 0.4, 0.4, 0.4, 1.0);
     float shininess = 5.0;
    vec4 light_position = vec4(0.0, 3.0, 3.0, 1.0);
    vec4 ambient;
     vec4 diffuse;
     vec4 specular;
     vec4 eyePosition = ModelViewMatrix * vPosition;
     vec4 eyeLightPos = light_position;
     vec3 N = normalize((ModelViewMatrix * vNormal).xyz);
     vec3 L = normalize(eyeLightPos.xyz - eyePosition.xyz);
     vec3 E = -normalize(eyePosition.xyz);
     vec3 H = normalize(L + E);
     float Kd = max(dot(L, N), 0.0);
    float Ks = pow(max(dot(N, H), 0.0), shininess);
    float Ka = 0.0;

    ambient = Ka*ambient_product;
    diffuse = Kd*diffuse_product;
    specular = Ks*specular_product;
    color = ambient+diffuse+specular;
}
