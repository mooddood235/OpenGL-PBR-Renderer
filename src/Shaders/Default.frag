#version 460 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

out vec4 FragColor;

uniform sampler2D texture_diffuse[3];
uniform sampler2D texture_specular[3];

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 viewMatrix;

void main()
{
    vec3 fragNorm = normalize(fragNormal);;
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStrength = max(dot(fragNorm, lightDir), 0.0);
    float specularStrength = pow(max(dot(fragNorm, halfwayDir), 0.0), 32);

    vec3 diffuse = vec3(texture(texture_diffuse[0], fragUV));
    vec3 specular = vec3(texture(texture_specular[0], fragUV));

   //FragColor = vec4(specular, 1);
    FragColor = vec4(diffuse * diffuseStrength + specular * specularStrength, 1.0);
} 