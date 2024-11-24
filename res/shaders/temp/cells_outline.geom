#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 g2f_barycentric;

void main() {
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
	g2f_barycentric = vec3(0.0, 0.0, 0.0);
	g2f_barycentric[i] = 1.0;
        EmitVertex();
    }
    EndPrimitive();
}
