#version 330 core

// Data from vertex shader.
in vec3 iPosWorld;
in vec3 iNormalWorld;

out vec4 FragColor;


void main()
{
    vec3 N = normalize(iNormalWorld);
    FragColor = vec4(N, 1.0);
}

