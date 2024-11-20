#version 330 core
out vec4 f_color;

in vec3 v2f_position;
in vec4 v2f_color;

void main() {
	f_color = v2f_color;
}