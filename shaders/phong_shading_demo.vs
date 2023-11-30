#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

// Transformation matrix.
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform mat4 MVP;

// Data pass to fragment shader.
out vec3 vPosition;
out vec3 vNormal;

void main()
{
    gl_Position = MVP * vec4(Position, 1.0);

    vec4 tmpPos = viewMatrix * worldMatrix * vec4(Position, 1.0);
    vPosition = vec3(tmpPos) / tmpPos.w;
    vNormal = normalize(vec3(normalMatrix * vec4(Normal, 0.0)));
}