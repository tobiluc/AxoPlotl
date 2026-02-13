#shader vertex
#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_data;
layout (location = 2) in vec3 v_cell_incenter;
layout (location = 3) in float v_cell_index;
 
uniform mat4 model_view_projection_matrix;
uniform float cell_scale;

uniform vec2 visible_data_range;
uniform vec4 min_color;
uniform vec4 max_color;
uniform bool use_data_as_color;

uniform vec4 clip_plane;

out vec4 v2f_color;

void main()
{
	vec4 model_position = vec4(v_cell_incenter + cell_scale * (v_position - v_cell_incenter), 1.0);

	gl_ClipDistance[0] = dot(model_position, clip_plane);

	gl_Position = model_view_projection_matrix * model_position;
	
	if (use_data_as_color) {
		v2f_color = v_data;
	} else if (v_data.x >= visible_data_range.x && v_data.x <= visible_data_range.y) {
		float t = (v_data.x - visible_data_range.x) / (visible_data_range.y - visible_data_range.x);
		t = clamp(t,0,1);
		v2f_color = mix(min_color, max_color, t);
	} else {
		v2f_color = vec4(0,0,0,0);
	}
} 

#shader fragment
#version 330 core
out vec4 f_color;

in vec4 v2f_color;
  
void main()
{
	if (v2f_color.a==0) {discard;}
	f_color = v2f_color; 
}