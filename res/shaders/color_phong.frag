#version 330 core
out vec4 f_color;

// Note that our calculations happen in view space

in vec4 g2f_position;
in vec4 g2f_color;
in vec3 g2f_normal;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
  
void main() {
	f_color = g2f_color;

	float shininess = 0.5;

	// Ambient
    	vec3 ambient = light.ambient * f_color.xyz;

	// Diffuse
	vec3 normal = normalize(g2f_normal);
	vec3 direction_to_light = normalize(light.position - g2f_position.xyz); 
	vec3 diffuse = light.diffuse * max(dot(normal, direction_to_light), 0.0) * f_color.xyz;

	// Specular
	vec3 direction_to_camera = normalize(-g2f_position.xyz);
	vec3 r = reflect(-direction_to_light, normal);
	vec3 specular = light.specular * pow(max(dot(direction_to_camera, r), 0.0), shininess); 

	f_color = vec4(ambient + diffuse + specular, 1.0);
}