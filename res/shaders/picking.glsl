#shader vertex
#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;

uniform mat4 model_view_projection_matrix;

out uvec3 v2f_color;

void main()
{
	gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
} 

#shader fragment
#version 330 core

out uvec3 f_color;

uniform uint object_index;
uniform uint draw_index;
  
void main()
{
	f_color = uvec3(object_index, draw_index, gl_PrimitiveID);
}