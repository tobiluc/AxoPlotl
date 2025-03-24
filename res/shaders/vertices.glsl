#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position; // in model space
layout (location = 1) in vec4 v_color;
 
uniform mat4 model_view_projection_matrix;
uniform float time;
uniform float point_size;

out vec4 v2f_color;

void main() {
	gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
	gl_PointSize = point_size;
	v2f_color = v_color;
} 

#shader fragment
#version 330 core
out vec4 f_color;

in vec4 v2f_color;

void main() {
	// Draw Points round
	vec2 d = 2.0 * gl_PointCoord - 1.0;
	if (dot(d, d) > 1.0) {discard;}

	f_color = v2f_color;
}