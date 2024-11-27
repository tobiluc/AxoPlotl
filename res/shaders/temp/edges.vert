#version 330 core

layout (location = 0) in vec3 v_position; // in model space
layout (location = 1) in vec3 v_color;
 
uniform mat4 model_view_projection_matrix;
uniform float time;

out vec3 v2g_color;

void main() {
	gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
	v2g_color = v_color;
} 