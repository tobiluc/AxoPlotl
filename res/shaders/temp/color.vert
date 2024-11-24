#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_normal;
 
// The following are in view space
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform float time;

out vec4 v2g_color;
out vec4 v2g_position;
out vec3 v2g_normal;

void main() {
	mat4 modelview_matrix = view_matrix * model_matrix;
        mat4 modelview_projection_matrix = projection_matrix * modelview_matrix;
	mat3 normal_matrix = mat3(transpose(inverse(modelview_matrix)));
	vec3 pos = v_position;
	gl_Position = modelview_projection_matrix * vec4(pos, 1.0);
	v2g_position = view_matrix * vec4(pos, 1.0);
	v2g_color = v_color;
	v2g_normal = normalize(normal_matrix * v_normal);
} 