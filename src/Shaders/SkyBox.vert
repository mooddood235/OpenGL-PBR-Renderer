#version 460 core
layout (location = 0) in vec3 inPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 localPos;

void main()
{
    localPos = inPos;

    mat4 rotView = mat4(mat3(viewMatrix)); // remove translation from the view matrix
    vec4 clipPos = projectionMatrix * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}