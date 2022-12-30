#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 uv;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPos, 1);
    fragPos = vec3(modelMatrix * vec4(inPos, 1));
    fragUV = uv;
    fragNormal = normalMatrix * inNormal;
}