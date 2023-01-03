#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 4) in vec2 uv;

out vec2 TexCoords;


void main(){
	gl_Position = vec4(inPos, 1);
	TexCoords = uv;
}