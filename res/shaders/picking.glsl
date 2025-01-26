#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 3) in float v_element_index;

uniform mat4 model_view_projection_matrix;

out float v2f_element_index;

void main()
{
	gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
	v2f_element_index = v_element_index;
} 

#shader fragment
#version 330 core

in float v2f_element_index;

out uvec3 f_color;

uniform uint mesh_index;
  
void main()
{
	f_color = uvec3(mesh_index, uint(v2f_element_index), gl_PrimitiveID);
}