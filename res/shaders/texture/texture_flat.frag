#version 330 core
out vec4 f_color;

in vec3 v2f_frag_position;
in vec2 v2f_texcoord;
in vec3 v2f_normal;

struct Material {
    sampler2D diffuse;
};
uniform Material material;

  
void main() {
	f_color = texture(material.diffuse, v2f_texcoord);
}