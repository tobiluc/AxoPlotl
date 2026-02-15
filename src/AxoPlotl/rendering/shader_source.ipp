#pragma once

const char* tbo_vertices_shader_src =
R"(
#shader vertex
#version 410 core

uniform samplerBuffer positions;
uniform samplerBuffer vertex_data;

uniform mat4 model_view_projection_matrix;

struct ClipBox {
    bool enabled;
    vec3 min;
    vec3 max;
};
uniform ClipBox clip_box;

struct Property {
    vec2 range;
    int visualization;
    sampler2D colormap;
};
uniform Property property;

uniform float point_size;

layout (location = 0) in uint v_vertex_index;

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
    vec3 pos = texelFetch(positions, int(v_vertex_index)).xyz;
    gl_Position = model_view_projection_matrix * vec4(pos, 1);
    gl_PointSize = point_size;

    // Clip Box
    vec3 dmin = pos - clip_box.min;
    vec3 dmax = clip_box.max - pos;
    gl_ClipDistance[1] = clip_box.enabled? min(dmin[0], dmax[0]) : 1.0;
    gl_ClipDistance[2] = clip_box.enabled? min(dmin[1], dmax[1]) : 1.0;
    gl_ClipDistance[3] = clip_box.enabled? min(dmin[2], dmax[2]) : 1.0;

    // Property Visualization
    vec4 prop_data = texelFetch(vertex_data, int(v_vertex_index));
    if (property.visualization == 0) { // scalar
        gl_ClipDistance[0] = min(prop_data.x - property.range.x, property.range.y - prop_data.x);
        float t = (prop_data.x - property.range.x) / (property.range.y - property.range.x);
        t = clamp(t,0,1);
        v2f_color = texture(property.colormap, vec2(t,0.5));
    } else if (property.visualization == 1) { // color
        gl_ClipDistance[0] = 1;
        v2f_color = prop_data;
    } else if (property.visualization == 2) { // 3d vector
        gl_ClipDistance[0] = 1;
        v2f_color = colorOnSphere(prop_data.xyz);
    } else {
        v2f_color = vec4(0,0,0,1);
    }
}

#shader fragment
#version 410 core
out vec4 f_color;

in vec4 v2f_color;

void main() {
    vec2 d = 2.0 * gl_PointCoord - 1.0;
    if (dot(d, d) > 1.0) {discard;}
    f_color = v2f_color;
}

)";

const char* tbo_edges_shader_src =
R"(
#shader vertex
#version 410 core

uniform samplerBuffer positions;
uniform samplerBuffer edge_data;

uniform mat4 model_view_projection_matrix;

struct ClipBox {
    bool enabled;
    vec3 min;
    vec3 max;
};
uniform ClipBox clip_box;

struct Property {
    vec2 range;
    int visualization;
    sampler2D colormap;
};
uniform Property property;

uniform float line_width;

layout (location = 0) in uint v_vertex_index;
layout (location = 1) in uint v_edge_index;

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
    vec3 pos = texelFetch(positions, int(v_vertex_index)).xyz;
    gl_Position = model_view_projection_matrix * vec4(pos, 1);

    // Clip Box
    vec3 dmin = pos - clip_box.min;
    vec3 dmax = clip_box.max - pos;
    v2g_clip_distances[1] = clip_box.enabled? min(dmin[0], dmax[0]) : 1.0;
    v2g_clip_distances[2] = clip_box.enabled? min(dmin[1], dmax[1]) : 1.0;
    v2g_clip_distances[3] = clip_box.enabled? min(dmin[2], dmax[2]) : 1.0;

    // Property Visualization
    vec4 prop_data = texelFetch(edge_data, int(v_edge_index));
    if (property.visualization == 0) { // scalar
        v2g_clip_distances[0] = min(prop_data.x - property.range.x, property.range.y - prop_data.x);
        float t = (prop_data.x - property.range.x) / (property.range.y - property.range.x);
        t = clamp(t,0,1);
        v2g_color = texture(property.colormap, vec2(t,0.5));
    } else if (property.visualization == 1) { // color
        v2g_clip_distances[0] = 1;
        v2g_color = prop_data;
    } else if (property.visualization == 2) { // 3d vector
        v2g_clip_distances[0] = 1;
        v2g_color = colorOnSphere(prop_data.xyz);
    } else {
        v2g_color = vec4(0,0,0,1);
    }
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
#version 410 core
out vec4 f_color;

in vec4 g2f_color;

void main() {
    f_color = g2f_color;
}

)";

const char* tbo_faces_shader_src =
R"(
#shader vertex
#version 410 core

uniform samplerBuffer positions;
uniform samplerBuffer face_data;

uniform mat4 model_view_projection_matrix;

struct ClipBox {
    bool enabled;
    vec3 min;
    vec3 max;
};
uniform ClipBox clip_box;

struct Property {
    vec2 range;
    int visualization;
    sampler2D colormap;
};
uniform Property property;

layout (location = 0) in uint v_vertex_index;
layout (location = 1) in uint v_face_index;

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
    vec3 pos = texelFetch(positions, int(v_vertex_index)).xyz;
    gl_Position = model_view_projection_matrix * vec4(pos, 1);

    // Clip Box
    vec3 dmin = pos - clip_box.min;
    vec3 dmax = clip_box.max - pos;
    gl_ClipDistance[1] = clip_box.enabled? min(dmin[0], dmax[0]) : 1.0;
    gl_ClipDistance[2] = clip_box.enabled? min(dmin[1], dmax[1]) : 1.0;
    gl_ClipDistance[3] = clip_box.enabled? min(dmin[2], dmax[2]) : 1.0;

    // Property Visualization
    vec4 prop_data = texelFetch(face_data, int(v_face_index));
    if (property.visualization == 0) { // scalar
        gl_ClipDistance[0] = min(prop_data.x - property.range.x, property.range.y - prop_data.x);
        float t = (prop_data.x - property.range.x) / (property.range.y - property.range.x);
        t = clamp(t,0,1);
        v2f_color = texture(property.colormap, vec2(t,0.5));
    } else if (property.visualization == 1) { // color
        gl_ClipDistance[0] = 1;
        v2f_color = prop_data;
    } else if (property.visualization == 2) { // 3d vector
        gl_ClipDistance[0] = 1;
        v2f_color = colorOnSphere(prop_data.xyz);
    } else {
        v2f_color = vec4(0,0,0,1);
    }
}

#shader fragment
#version 410 core
out vec4 f_color;

in vec4 v2f_color;

void main() {
    f_color = v2f_color;
}

)";

const char* tbo_cells_shader_src =
R"(
#shader vertex
#version 410 core

uniform samplerBuffer positions;
uniform samplerBuffer cell_data;

uniform mat4 model_view_projection_matrix;

struct ClipBox {
    bool enabled;
    vec3 min;
    vec3 max;
};
uniform ClipBox clip_box;

struct Property {
    vec2 range;
    int visualization;
    sampler2D colormap;
};
uniform Property property;

uniform float cell_scale;

layout (location = 0) in uint v_vertex_index;
layout (location = 1) in uint v_cell_index;
layout (location = 2) in vec3 v_cell_incenter;

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
    vec3 pos = texelFetch(positions, int(v_vertex_index)).xyz;
    gl_Position = model_view_projection_matrix * vec4(
        v_cell_incenter + cell_scale*(pos - v_cell_incenter), 1);

    // Clip Box
    vec3 dmin = pos - clip_box.min;
    vec3 dmax = clip_box.max - pos;
    gl_ClipDistance[1] = clip_box.enabled? min(dmin[0], dmax[0]) : 1.0;
    gl_ClipDistance[2] = clip_box.enabled? min(dmin[1], dmax[1]) : 1.0;
    gl_ClipDistance[3] = clip_box.enabled? min(dmin[2], dmax[2]) : 1.0;

    // Property Visualization
    vec4 prop_data = texelFetch(cell_data, int(v_cell_index));
    if (property.visualization == 0) { // scalar
        gl_ClipDistance[0] = min(prop_data.x - property.range.x, property.range.y - prop_data.x);
        float t = (prop_data.x - property.range.x) / (property.range.y - property.range.x);
        t = clamp(t,0,1);
        v2f_color = texture(property.colormap, vec2(t,0.5));
    } else if (property.visualization == 1) { // color
        gl_ClipDistance[0] = 1;
        v2f_color = prop_data;
    } else if (property.visualization == 2) { // 3d vector
        gl_ClipDistance[0] = 1;
        v2f_color = colorOnSphere(prop_data.xyz);
    } else {
        v2f_color = vec4(0,0,0,1);
    }
}

#shader fragment
#version 410 core
out vec4 f_color;

in vec4 v2f_color;

void main() {
    f_color = v2f_color;
}

)";

const char* tbo_faces_picking_shader_src =
R"(
#shader vertex
#version 410 core

uniform samplerBuffer positions;

uniform mat4 model_view_projection_matrix;

layout (location = 0) in uint v_vertex_index;
layout (location = 1) in uint v_face_index;

out float v2f_face_index;

void main()
{
    vec3 pos = texelFetch(positions, int(v_vertex_index)).xyz;
    gl_Position = model_view_projection_matrix * vec4(pos, 1.0);
    v2f_face_index = v_face_index;
}

#shader fragment
#version 410 core

in float v2f_face_index;

out uvec3 f_color;

uniform uint object_index;

void main()
{
    f_color = uvec3(object_index, uint(v2f_face_index), gl_PrimitiveID);
}
)";

const char* tbo_ells_outline_shader_src =
R"(
#shader vertex
#version 410 core

uniform samplerBuffer positions;
uniform samplerBuffer cell_data;

layout (location = 0) in uint v_vertex_index;
layout (location = 1) in uint v_cell_index;
layout (location = 2) in vec3 v_cell_incenter;

uniform mat4 model_view_projection_matrix;
uniform float cell_scale;
uniform vec4 outline_color;

struct ClipBox {
    bool enabled;
    vec3 min;
    vec3 max;
};
uniform ClipBox clip_box;

struct Property {
    vec2 range;
    int visualization;
    sampler2D colormap;
};
uniform Property property;

out vec4 v2f_color;

void main()
{
    vec3 pos = texelFetch(positions, int(v_vertex_index)).xyz;
    gl_Position = model_view_projection_matrix * vec4(
        v_cell_incenter + cell_scale*(pos - v_cell_incenter), 1);

    // Clip Box
    vec3 dmin = pos - clip_box.min;
    vec3 dmax = clip_box.max - pos;
    gl_ClipDistance[1] = clip_box.enabled? min(dmin[0], dmax[0]) : 1.0;
    gl_ClipDistance[2] = clip_box.enabled? min(dmin[1], dmax[1]) : 1.0;
    gl_ClipDistance[3] = clip_box.enabled? min(dmin[2], dmax[2]) : 1.0;

    // Property Visualization
    vec4 prop_data = texelFetch(cell_data, int(v_cell_index));
    if (property.visualization == 0) { // scalar
        gl_ClipDistance[0] = min(prop_data.x - property.range.x, property.range.y - prop_data.x);
    } else {
        gl_ClipDistance[0] = 1;
    }
    v2f_color = outline_color;
}

#shader fragment
#version 410 core
out vec4 f_color;

in vec4 v2f_color;

void main()
{
    f_color = v2f_color;
}
)";
