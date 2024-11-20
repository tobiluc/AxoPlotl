#version 330 core
out vec4 f_color;

in vec3 v2f_position;

uniform vec4 color;

void main() {
	f_color = color;
}