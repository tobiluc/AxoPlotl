#shader vertex
#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in vec3 v_normal;
 
uniform mat4 view_matrix;
uniform mat4 model_view_projection_matrix;
uniform mat3 normal_matrix;
uniform float time;

out vec3 v2f_color;
out vec4 v2f_view_position; // in view space
out vec3 v2f_view_normal;

void main()
{
	vec3 pos = v_position;
	gl_Position = model_view_projection_matrix * vec4(pos, 1.0);

	v2f_view_position = view_matrix * vec4(pos, 1.0);
	v2f_color = v_color;
	v2f_view_normal = normalize(normal_matrix * v_normal);
} 

#shader fragment
#version 330 core
out vec4 f_color;

in vec4 v2f_view_position;
in vec3 v2f_color;
in vec3 v2f_view_normal;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

uniform float outline_width;
uniform vec3 outline_color;
  
void main()
{

	f_color = vec4(v2f_color, 1.0);

	float shininess = 0.5;

	// Ambient
    	vec3 ambient = light.ambient * f_color.xyz;

	// Diffuse
	vec3 normal = normalize(v2f_view_normal);
	vec3 direction_to_light = normalize(light.position - v2f_view_position.xyz); 
	vec3 diffuse = light.diffuse * max(dot(normal, direction_to_light), 0.0) * f_color.xyz;

	// Specular
	vec3 direction_to_camera = normalize(-v2f_view_position.xyz);
	vec3 r = reflect(-direction_to_light, normal);
	vec3 specular = light.specular * pow(max(dot(direction_to_camera, r), 0.0), shininess); 

	f_color = vec4(ambient + diffuse + specular, 1.0);
}