#version 330 core
out vec4 f_color;

in vec4 g2f_position;
in vec4 g2f_color;
in vec3 g2f_normal;

void main() {
	f_color = g2f_color;
}