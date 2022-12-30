#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBiTangent;
layout (location = 4) in vec2 inUV;


out vec3 fragPos;
out vec2 fragUV;

out mat3 TBN;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPos, 1);
    fragPos = vec3(modelMatrix * vec4(inPos, 1));
    fragUV = inUV;

    vec3 fragNormal = normalize(normalMatrix * inNormal);
    vec3 fragTangent = normalize(normalMatrix * inTangent);
    vec3 fragBiTangent = normalize(normalMatrix * inBiTangent);

    TBN = inverse(mat3(transpose(mat3(fragTangent, fragBiTangent, fragNormal))));
}