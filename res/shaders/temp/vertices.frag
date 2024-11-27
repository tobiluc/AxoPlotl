#version 330 core
out vec4 f_color;

in vec3 v2f_color;

void main() {
	// Draw Points round
	vec2 d = 2.0 * gl_PointCoord - 1.0;
	if (dot(d, d) > 1.0) {discard;}

	f_color = vec4(v2f_color, 1.0);
}