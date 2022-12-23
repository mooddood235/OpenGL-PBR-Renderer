#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

out vec4 fragColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPos, 1);
    fragColor = normalize(modelMatrix * vec4(inNormal, 1));
}