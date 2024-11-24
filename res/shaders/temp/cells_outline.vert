#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec3 v_cell_center;

uniform mat4 model_view_projection_matrix;
uniform float cell_scale;

void main() {
	vec3 d = v_position - v_cell_center;
	vec3 pos = v_cell_center + cell_scale * d;
	gl_Position = model_view_projection_matrix * vec4(pos, 1.0);
} 