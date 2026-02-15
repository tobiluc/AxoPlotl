#pragma once

const char* vertices_shader_src =
R"(
#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position; // in model space
layout (location = 1) in vec4 v_data; // [property value, ?, ?, ?]

uniform sampler2D colormap;

uniform mat4 model_view_projection_matrix;
uniform vec2 visible_data_range;
uniform int data_type;
uniform float point_size;

uniform bool clip_box_enabled;
uniform vec3 clip_box_min;
uniform vec3 clip_box_max;

out vec4 v2f_color;

vec4 colorOnSphere(vec3 p) {
    float d = length(p);
        return vec4(
            0.5 * (p.x/d + 1),
            0.5 * (p.y/d + 1),
            0.5 * (p.z/d + 1),
            1
        );
}

void main() {
    if (data_type == 0) {
        // use v_data as scalar
        gl_ClipDistance[0] = min(v_data.x - visible_data_range.x, visible_data_range.y - v_data.x);

        float t = (v_data.x - visible_data_range.x) / (visible_data_range.y - visible_data_range.x);
        t = clamp(t,0,1);
        v2f_color = texture(colormap, vec2(t,0.5));

    } else if (data_type == 1) {
        // use v_data as color
        gl_ClipDistance[0] = 0;
        v2f_color = v_data;


    } else if (data_type == 2) {
        // use v_data as 3d vector
        gl_ClipDistance[0] = 0;
        v2f_color = colorOnSphere(v_data.xyz);
    }

    vec3 dmin = v_position - clip_box_min;
    vec3 dmax = clip_box_max - v_position;
    gl_ClipDistance[1] = clip_box_enabled? min(dmin[0], dmax[0]) : 1.0;
    gl_ClipDistance[2] = clip_box_enabled? min(dmin[1], dmax[1]) : 1.0;
    gl_ClipDistance[3] = clip_box_enabled? min(dmin[2], dmax[2]) : 1.0;

    gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
    gl_PointSize = point_size;
}

#shader fragment
#version 330 core
out vec4 f_color;

in vec4 v2f_color;

void main() {
    // Draw Points round
    vec2 d = 2.0 * gl_PointCoord - 1.0;
    if (dot(d, d) > 1.0) {discard;}

    f_color = v2f_color;
}
)";

const char* edges_shader_src =
R"(
#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position; // in model space
layout (location = 1) in vec4 v_data;

uniform sampler2D colormap;

uniform mat4 model_view_projection_matrix;
uniform vec2 visible_data_range;
uniform int data_type;

uniform bool clip_box_enabled;
uniform vec3 clip_box_min;
uniform vec3 clip_box_max;

out vec4 v2g_color;
out vec4 v2g_clip_distances;

vec4 colorOnSphere(vec3 p) {
    float d = length(p);
        return vec4(
            0.5 * (p.x/d + 1),
            0.5 * (p.y/d + 1),
            0.5 * (p.z/d + 1),
            1
        );
}

void main() {
    if (data_type == 0) {
        // use v_data as scalar
        v2g_clip_distances[0] = min(v_data.x - visible_data_range.x, visible_data_range.y - v_data.x);

        float t = (v_data.x - visible_data_range.x) / (visible_data_range.y - visible_data_range.x);
        t = clamp(t,0,1);
        v2g_color = texture(colormap, vec2(t,0.5));

    } else if (data_type == 1) {
        // use v_data as color
        v2g_clip_distances[0] = 0;

        v2g_color = v_data;
    } else if (data_type == 2) {
        // use v_data as 3d vector
        v2g_clip_distances[0] = 0;
        v2g_color = colorOnSphere(v_data.xyz);
    }

    vec3 dmin = v_position - clip_box_min;
    vec3 dmax = clip_box_max - v_position;
    v2g_clip_distances[1] = clip_box_enabled? min(dmin[0], dmax[0]) : 1.0;
    v2g_clip_distances[2] = clip_box_enabled? min(dmin[1], dmax[1]) : 1.0;
    v2g_clip_distances[3] = clip_box_enabled? min(dmin[2], dmax[2]) : 1.0;

    gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
}

#shader geometry
#version 330 core

// Transforms a Line into a triangle strip to make it appear thicker

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float line_width;
uniform vec2 inverse_viewport_size;

in vec4 v2g_color[];
in vec4 v2g_clip_distances[];

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

    gl_ClipDistance[0] = v2g_clip_distances[i/2][0];
    gl_ClipDistance[1] = v2g_clip_distances[i/2][1];
    gl_ClipDistance[2] = v2g_clip_distances[i/2][2];
    gl_ClipDistance[3] = v2g_clip_distances[i/2][3];


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
)";

const char* faces_shader_src =
R"(
#shader vertex
#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_data;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in float v_face_index;

uniform sampler2D colormap;

uniform mat4 model_view_projection_matrix;
uniform vec2 visible_data_range;
uniform int data_type;

uniform bool clip_box_enabled;
uniform vec3 clip_box_min;
uniform vec3 clip_box_max;

out vec4 v2f_color;

vec4 colorOnSphere(vec3 p) {
    float d = length(p);
        return vec4(
            0.5 * (p.x/d + 1),
            0.5 * (p.y/d + 1),
            0.5 * (p.z/d + 1),
            1
        );
}

void main()
{
    if (data_type == 0) {
        // use v_data as scalar
        gl_ClipDistance[0] = min(v_data.x - visible_data_range.x, visible_data_range.y - v_data.x);

        float t = (v_data.x - visible_data_range.x) / (visible_data_range.y - visible_data_range.x);
        t = clamp(t,0,1);
        v2f_color = texture(colormap, vec2(t,0.5));

    } else if (data_type == 1) {
        // use v_data as color
        gl_ClipDistance[0] = 0;
        v2f_color = v_data;
    } else if (data_type == 2) {
        // use v_data as 3d vector
        gl_ClipDistance[0] = 0;
        v2f_color = colorOnSphere(v_data.xyz);
    }

    vec3 dmin = v_position - clip_box_min;
    vec3 dmax = clip_box_max - v_position;
    gl_ClipDistance[1] = clip_box_enabled? min(dmin[0], dmax[0]) : 1.0;
    gl_ClipDistance[2] = clip_box_enabled? min(dmin[1], dmax[1]) : 1.0;
    gl_ClipDistance[3] = clip_box_enabled? min(dmin[2], dmax[2]) : 1.0;

    gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
}

#shader fragment
#version 330 core
out vec4 f_color;

in vec4 v2f_color;

void main()
{
    f_color = v2f_color;
}
)";

const char* faces_picking_shader_src =
R"(
#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 3) in float v_buffer;

uniform mat4 model_view_projection_matrix;

out float v2f_buffer;

void main()
{
    gl_Position = model_view_projection_matrix * vec4(v_position, 1.0);
    v2f_buffer = v_buffer;
}

#shader fragment
#version 330 core

in float v2f_buffer;

out uvec3 f_color;

uniform uint object_index;

void main()
{
    f_color = uvec3(object_index, uint(v2f_buffer), gl_PrimitiveID);
}
)";

const char* cells_shader_src =
R"(
#shader vertex
#version 330 core

// The following are in model space
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_data;
layout (location = 2) in vec3 v_cell_incenter;
layout (location = 3) in float v_cell_index;

uniform mat4 model_view_projection_matrix;
uniform float cell_scale;

uniform sampler2D colormap;
uniform vec2 visible_data_range;
uniform int data_type;

uniform bool clip_box_enabled;
uniform vec3 clip_box_min;
uniform vec3 clip_box_max;

out vec4 v2f_color;

vec4 colorOnSphere(vec3 p) {
    float d = length(p);
        return vec4(
            0.5 * (p.x/d + 1),
            0.5 * (p.y/d + 1),
            0.5 * (p.z/d + 1),
            1
        );
}

void main()
{

    if (data_type == 0) {
        // use v_data as scalar
        gl_ClipDistance[0] = min(v_data.x - visible_data_range.x, visible_data_range.y - v_data.x);

        float t = (v_data.x - visible_data_range.x) / (visible_data_range.y - visible_data_range.x);
        t = clamp(t,0,1);
        v2f_color = texture(colormap, vec2(t,0.5));

    } else if (data_type == 1) {
        // use v_data as color
        gl_ClipDistance[0] = 0;

        v2f_color = v_data;
    } else if (data_type == 2) {
        // use v_data as 3d vector
        gl_ClipDistance[0] = 0;
        v2f_color = colorOnSphere(v_data.xyz);
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
    f_color = v2f_color;
}
)";

const char* cells_outline_shader_src =
R"(
#shader vertex
#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_data;
layout (location = 2) in vec3 v_cell_incenter;

uniform mat4 model_view_projection_matrix;
uniform float cell_scale;
uniform vec4 color;

uniform vec2 visible_data_range;
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
    } else if (data_type == 1) {
        // use v_data as color
        gl_ClipDistance[0] = 0;
    } else if (data_type == 2) {
        // use v_data as 3d vector
        gl_ClipDistance[0] = 0;
    }

    vec3 dmin = v_position - clip_box_min;
    vec3 dmax = clip_box_max - v_position;
    gl_ClipDistance[1] = clip_box_enabled? min(dmin[0], dmax[0]) : 1.0;
    gl_ClipDistance[2] = clip_box_enabled? min(dmin[1], dmax[1]) : 1.0;
    gl_ClipDistance[3] = clip_box_enabled? min(dmin[2], dmax[2]) : 1.0;

    vec4 model_position = vec4(v_cell_incenter + cell_scale * (v_position - v_cell_incenter), 1.0);
    gl_Position = model_view_projection_matrix * model_position;
    v2f_color = color;
}

#shader fragment
#version 330 core
out vec4 f_color;

in vec4 v2f_color;

void main()
{
    f_color = v2f_color;
}
)";
