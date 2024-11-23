#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;
 
// The following are in view space
uniform mat4 view_matrix;
uniform mat4 modelview_projection_matrix;
uniform mat3 normal_matrix;

out vec3 v2f_frag_position;
out vec3 v2f_normal;
out vec2 v2f_texcoord;

void main() {
	gl_Position = modelview_projection_matrix * vec4(v_position, 1.0);

	v2f_normal = normalize(normal_matrix * v_normal);
	v2f_texcoord = v_texcoord;
	v2f_frag_position = vec3(view_matrix * vec4(v_position, 1.0));
} 