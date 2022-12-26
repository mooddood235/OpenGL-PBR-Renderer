#version 460 core

in vec2 fragUV;

out vec4 FragColor;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;

uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main()
{
    FragColor = texture(texture_diffuse0, fragUV);
} 