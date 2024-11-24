#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec3 v_cell_center;

uniform mat4 view_matrix;
uniform mat4 model_view_projection_matrix;
uniform mat3 normal_matrix;
uniform float time;
uniform float cell_scale;

out vec3 v2g_color;
out vec4 v2g_view_position;
out vec3 v2g_view_normal;

void main()
{
	vec3 pos = v_cell_center + cell_scale * (v_position - v_cell_center);
	gl_Position = model_view_projection_matrix * vec4(pos, 1.0);

	v2g_view_position = view_matrix * vec4(pos, 1.0);
	v2g_color = v_color;
	v2g_view_normal = normalize(normal_matrix * v_normal);
} 