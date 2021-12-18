#version 430

in vec3 EyeNormal;       // Normal in eye coordinates
in vec4 EyePosition;     // Position in eye coordinates
in vec4 ProjTexCoord;

layout(binding=0) uniform sampler2D ProjectorTex;

uniform struct LightInfo {
  vec4 Position;  // Light position in cam. coords.
  vec3 L;         // D,S intensity
  vec3 La;        // Amb intensity
} Light;

uniform struct MaterialInfo {
  vec3 Ka;        // Ambient reflectivity
  vec3 Kd;        // Diffuse reflectivity
  vec3 Ks;        // Specular reflectivity
  float Shininess;// Specular shininess factor
} Material;

layout(location = 0) out vec4 FragColor;

vec3 blinnPhong(vec3 position, vec3 n)
    {
    vec3 ambient = Light.La * Material.Ka;
    vec3 s = normalize(Light.Position.xyz - position);
    float sDotN = max(dot(s,n), 0.0);
    vec3 diffuse = Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if(sDotN > 0.0)
        {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h,n), 0.0), Material.Shininess);
        }
    return ambient + Light.L * (diffuse + spec);
    }

void main()
    {
    vec3 color = blinnPhong(EyePosition.xyz, normalize(EyeNormal));
    vec3 projTexColor = vec3(0.0);
    if(ProjTexCoord.z > 0.0)
        projTexColor = 0.5*textureProj(ProjectorTex, ProjTexCoord).rgb;
    FragColor = vec4(color + projTexColor, 1);
    }
