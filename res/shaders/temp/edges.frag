#version 330 core
out vec4 f_color;

in vec3 g2f_color;

void main() {
	f_color = vec4(g2f_color, 1.0);
}