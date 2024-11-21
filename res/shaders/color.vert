#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_normal;
 
// The following are in view space
uniform mat4 view_matrix;
uniform mat4 modelview_projection_matrix;
uniform mat3 normal_matrix;
uniform float time;

out vec4 v2f_color;
out vec3 v2f_position;
out vec3 v2f_normal;

void main() {
	vec3 pos = v_position;
	gl_Position = modelview_projection_matrix * vec4(pos, 1.0);
	v2f_position = vec3(view_matrix * vec4(pos, 1.0));
	v2f_color = v_color;
	v2f_normal = normalize(normal_matrix * v_normal);
} 