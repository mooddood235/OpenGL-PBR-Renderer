#version 460 core
layout (location = 0) in vec3 inPos;
  
out vec4 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPos, 1);
    vertexColor = vec4(0.5, 0.2, 0.3, 1);
}