#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position; // in model space
layout (location = 1) in vec4 v_color;
 
uniform mat4 model_view_projection_matrix;
uniform float time;

out vec4 v2g_color;

void main() {
	gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
	v2g_color = v_color;
} 

#shader geometry
#version 330 core

// Transforms a Line into a triangle strip to make it appear thicker

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float line_width;
uniform vec2 inverse_viewport_size;

in vec4 v2g_color[];

out vec4 g2f_color;

void main() {
    float r = line_width * 0.5;
    vec2 inv_viewport = vec2(0.1, 0.1);

    // These are in projection space!
    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

    vec2 dir = normalize(p2.xy - p1.xy);
    vec2 normal = vec2(dir.y, -dir.x);

    vec4 offset1, offset2;
    offset1 = vec4(normal * inverse_viewport_size * (r * p1.w), 0, 0);
    offset2 = vec4(normal * inverse_viewport_size * (r * p2.w), 0, 0);

    vec4 coords[4];
    coords[0] = p1 + offset1;
    coords[1] = p1 - offset1;
    coords[2] = p2 + offset2;
    coords[3] = p2 - offset2;

    for (int i = 0; i < 4; ++i) {
        gl_Position = coords[i];
	g2f_color = v2g_color[i/2];
        EmitVertex();
    }
    EndPrimitive();
}

#shader fragment
#version 330 core
out vec4 f_color;

in vec4 g2f_color;

void main() {
	f_color = g2f_color;
}