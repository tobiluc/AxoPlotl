#version 330 core

layout (location = 0) in vec3 v_position; // in model space
layout (location = 1) in vec3 v_color;
 
uniform mat4 model_view_projection_matrix;
uniform float time;
uniform float point_size;

out vec3 v2f_color;

void main() {
	gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
	gl_PointSize = point_size;
	v2f_color = v_color;
} 