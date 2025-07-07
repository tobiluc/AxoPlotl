#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 3) in float v_buffer;

uniform mat4 model_view_projection_matrix;

out float v2f_buffer;

void main()
{
	gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
	v2f_buffer = v_buffer;
} 

#shader fragment
#version 330 core

in float v2f_buffer;

out uvec3 f_color;

uniform uint object_index;
  
void main()
{
	f_color = uvec3(object_index, uint(v2f_buffer), gl_PrimitiveID);
}