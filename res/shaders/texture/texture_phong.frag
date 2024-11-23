#version 330 core
out vec4 f_color;

// Note that our calculations happen in view space

in vec3 v2f_frag_position;
in vec2 v2f_texcoord;
in vec3 v2f_normal;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
  
void main() {
	f_color = texture(material.diffuse, v2f_texcoord);

	// Ambient
    vec3 ambient = light.ambient * f_color.xyz;

	// Diffuse
	vec3 normal = normalize(v2f_normal);
	vec3 direction_to_light = normalize(light.position - v2f_frag_position); 
	vec3 diffuse = light.diffuse * max(dot(normal, direction_to_light), 0.0) * f_color.xyz;

	// Specular
	vec3 direction_to_camera = normalize(-v2f_frag_position);
	vec3 r = reflect(-direction_to_light, normal);
	vec3 specular = light.specular * pow(max(dot(direction_to_camera, r), 0.0), material.shininess) * texture(material.specular, v2f_texcoord).xyz; 

	f_color = vec4(ambient + diffuse + specular, 1.0);
}