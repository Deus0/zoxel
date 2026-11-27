layout(location=0) in vec3 vertex_position;
layout(location=1) in vec2 vertex_uv;
layout(location=2) in vec4 vertex_color;     // its light, its alpha its glow
const float GLOW = 3.0;   // a vertex's glow (its colour's alpha, doc/plans/hdr.md): up to this much past white

uniform mat4 mvp_matrix;
out vec2 uv;
out vec3 vertex_color_frag;

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    uv = vertex_uv;
    // a glowing face is lit fully (at night too), its glow how far past white
    vertex_color_frag = (vertex_color.a > 0.0 ? vec3(1.0) : vertex_color.rgb) * (1.0 + vertex_color.a * GLOW) * zox_sky_light(vertex_color.a);
}
