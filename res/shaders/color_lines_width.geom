#version 330 core

layout(lines) in;               // Two input vertices per line
layout(triangle_strip, max_vertices = 4) out;

in vec4 v2g_color[];               // Color passed from vertex shader
in vec4 v2g_position[];       // Camera-space positions
in vec3 v2g_normal[];

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform float lineWidth;        // Line width in pixels

out vec4 g2f_color;             // Pass to fragment shader
out vec4 g2f_position; 
out vec3 g2f_normal; 

void main() {
    // Input vertices in camera space
    vec4 p0 = v2g_position[0];
    vec4 p1 = v2g_position[1];

    // Transform to normalized device coordinates (NDC)
    vec4 ndcP0 = projection_matrix * p0;
    vec4 ndcP1 = projection_matrix * p1;

    // Homogeneous divide to get screen-space positions
    vec2 screenP0 = ndcP0.xy / ndcP0.w;
    vec2 screenP1 = ndcP1.xy / ndcP1.w;

    // Compute screen-space direction of the line
    vec2 screenDir = normalize(screenP1 - screenP0);

    // Compute a perpendicular vector in screen space
    vec2 screenPerp = vec2(-screenDir.y, screenDir.x);

    // Scale perpendicular vector by line width in screen space
    vec2 offset = screenPerp * (lineWidth / 2.0);

    // Emit quad vertices, converting back to clip space
    g2f_color = v2g_color[0];
    g2f_position = v2g_position[0];
    g2f_normal = v2g_normal[0];
    gl_Position = vec4(screenP0 + offset, ndcP0.zw);  // Top left
    EmitVertex();

    g2f_color = v2g_color[0];
    g2f_position = v2g_position[0];
    g2f_normal = v2g_normal[0];
    gl_Position = vec4(screenP0 - offset, ndcP0.zw);  // Bottom left
    EmitVertex();

    g2f_color = v2g_color[1];
    g2f_position = v2g_position[1];
    g2f_normal = v2g_normal[1];
    gl_Position = vec4(screenP1 + offset, ndcP1.zw);  // Top right
    EmitVertex();

    g2f_color = v2g_color[1];
    g2f_position = v2g_position[1];
    g2f_normal = v2g_normal[1];
    gl_Position = vec4(screenP1 - offset, ndcP1.zw);  // Bottom right
    EmitVertex();

    EndPrimitive();
}
