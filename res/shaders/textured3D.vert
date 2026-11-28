layout(location=0) in vec3 vertex_position;
layout(location=1) in vec2 vertex_uv;
layout(location=2) in vec4 vertex_color;     // its light (r the sky's, g blocks'), its alpha its glow
const float GLOW = 3.0;   // a vertex's glow (its colour's alpha, doc/plans/hdr.md): up to this much past white

uniform mat4 mvp_matrix;
out vec2 uv;
out vec3 vertex_color_frag;

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    uv = vertex_uv;
    // its light: r the sky's, g blocks' (lights3/sys/build_smooth.c); a glowing face lit fully
    vec3 light = vertex_color.a > 0.0 ? vec3(1.0) : zox_light(vertex_color.r, vertex_color.g);
    vertex_color_frag = light * (1.0 + vertex_color.a * GLOW);
}
