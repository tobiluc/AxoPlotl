#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 v2g_color[];
in vec4 v2g_view_position[];
in vec3 v2g_view_normal[];

out vec3 g2f_barycentric;
out vec3 g2f_color;
out vec4 g2f_view_position;
out vec3 g2f_view_normal;

void main() {
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
	g2f_barycentric = vec3(0.0, 0.0, 0.0);
	g2f_barycentric[i] = 1.0;
	g2f_color = v2g_color[i];
        g2f_view_position = v2g_view_position[i];
        g2f_view_normal = v2g_view_normal[i];
        EmitVertex();
    }
    EndPrimitive();
}
