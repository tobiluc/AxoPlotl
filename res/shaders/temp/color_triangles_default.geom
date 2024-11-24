#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

// Inputs from vertex shader
in vec4 v2g_position[];
in vec3 v2g_normal[];
in vec4 v2g_color[];

// Outputs to fragment shader
out vec4 g2f_position;
out vec3 g2f_normal;
out vec4 g2f_color;

void main() {
    for (int i = 0; i < 3; i++) {
        g2f_position = v2g_position[i];
        g2f_normal = v2g_normal[i];
        g2f_color = v2g_color[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
