#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position;

uniform mat4 model_view_projection_matrix;

void main()
{
	gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
} 

#shader fragment
#version 330 core

out uvec3 f_color;

uniform uint object_index;
uniform uint vao_index;
  
void main()
{
	f_color = uvec3(object_index, vao_index, gl_PrimitiveID);
}