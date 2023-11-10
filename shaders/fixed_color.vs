#version 330 core

layout (location = 0) in vec3 Position;

// uniform mat4 modelMatrix;
// uniform mat4 viewMatrix;
// uniform mat4 projMatrix;
uniform mat4 MVP;

void main()
{
    // gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(Position, 1.0);
    gl_Position = MVP * vec4(Position, 1.0);
}