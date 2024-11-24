#version 330 core
out vec4 f_color;

uniform vec3 color;
uniform float width;

in vec3 g2f_barycentric;
  
void main() {
	
    // Discard the fragment if it is too much in the triangle center
    float d = min(g2f_barycentric.x, min(g2f_barycentric.y, g2f_barycentric.z));
    vec3 grad = fwidth(g2f_barycentric);
    float threshold = width * min(grad.x, min(grad.y, grad.z));

    if (d < threshold) {
        f_color = vec4(color, 1.0);
    } else {
        discard;
    }
}