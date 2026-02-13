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
uniform int data_type;

uniform bool clip_box_enabled;
uniform vec3 clip_box_min;
uniform vec3 clip_box_max;

out vec4 v2f_color;

void main()
{

	if (data_type == 0) {
		// use v_data as scalar
		gl_ClipDistance[0] = min(v_data.x - visible_data_range.x, visible_data_range.y - v_data.x);

		float t = (v_data.x - visible_data_range.x) / (visible_data_range.y - visible_data_range.x);
		t = clamp(t,0,1);
		v2f_color = mix(min_color, max_color, t);
	
	} else if (data_type == 1) {
		// use v_data as color
		gl_ClipDistance[0] = 0;

		v2f_color = v_data;
	}

	vec3 dmin = v_position - clip_box_min;
	vec3 dmax = clip_box_max - v_position;
	gl_ClipDistance[1] = clip_box_enabled? min(dmin[0], dmax[0]) : 1.0;
	gl_ClipDistance[2] = clip_box_enabled? min(dmin[1], dmax[1]) : 1.0;
	gl_ClipDistance[3] = clip_box_enabled? min(dmin[2], dmax[2]) : 1.0;

	vec4 model_position = vec4(v_cell_incenter + cell_scale * (v_position - v_cell_incenter), 1.0);
	gl_Position = model_view_projection_matrix * model_position;
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