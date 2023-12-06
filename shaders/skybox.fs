#version 330 core

in vec2 iTexCoord;

// Material properties.
uniform sampler2D mapKd;

out vec4 FragColor;


void main()
{
    // We create the uv coordinate from the top, so don't need to inverse.
    FragColor = texture2D(mapKd, vec2(iTexCoord.x, 1.0 - iTexCoord.y));
}
