#version 460 core
layout (location = 0) in vec3 inPos;

out vec3 localPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
    localPos = inPos;  
    gl_Position =  projectionMatrix * viewMatrix * vec4(inPos, 1.0);
}