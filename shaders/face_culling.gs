#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec3 vColor[];

out vec3 fColor;

void main() {
    vec3 normal = normalize(cross(vPosition[1] - vPosition[0], vPosition[2] - vPosition[0]));
    if (dot(normal, vPosition[0]) > 0) {
        return;
    }

    for (int i = 0; i < 3; i++) {
        fColor = vColor[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}