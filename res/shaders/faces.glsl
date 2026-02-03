#shader vertex
#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in float v_face_index;
 
uniform mat4 model_view_projection_matrix;
uniform float time;
uniform bool use_global_color;
uniform vec4 global_color;

out vec4 v2f_color;

void main()
{
	vec3 pos = v_position;
	gl_Position = model_view_projection_matrix * vec4(pos, 1.0);

	if (use_global_color) {v2f_color = global_color;}
	else {v2f_color = v_color;}
} 

#shader fragment
#version 330 core
out vec4 f_color;

in vec4 v2f_color;
  
void main()
{
	f_color = v2f_color; 
}