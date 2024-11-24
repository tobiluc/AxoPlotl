#version 330 core
out vec4 f_color;

in vec4 g2f_view_position;
in vec3 g2f_color;
in vec3 g2f_view_normal;
in vec3 g2f_barycentric;

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
        // We are in the outline part of the triangle if d < threshold
       float dist = min(g2f_barycentric.x, min(g2f_barycentric.y, g2f_barycentric.z));
       vec3 grad = fwidth(g2f_barycentric);
       float threshold = outline_width * 0.5 * min(grad.x, min(grad.y, grad.z));
       if (dist < threshold)
	{
          f_color = vec4(outline_color, 1.0);
        }
	else
	{

	f_color = vec4(g2f_color, 1.0);

	float shininess = 0.5;

	// Ambient
    	vec3 ambient = light.ambient * f_color.xyz;

	// Diffuse
	vec3 normal = normalize(g2f_view_normal);
	vec3 direction_to_light = normalize(light.position - g2f_view_position.xyz); 
	vec3 diffuse = light.diffuse * max(dot(normal, direction_to_light), 0.0) * f_color.xyz;

	// Specular
	vec3 direction_to_camera = normalize(-g2f_view_position.xyz);
	vec3 r = reflect(-direction_to_light, normal);
	vec3 specular = light.specular * pow(max(dot(direction_to_camera, r), 0.0), shininess); 

	f_color = vec4(ambient + diffuse + specular, 1.0);

	}
}