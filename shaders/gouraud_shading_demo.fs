#version 330 core

in vec3 iLightingColor;

out vec4 FragColor;


void main()
{
    FragColor = vec4(iLightingColor, 1.0);
}
