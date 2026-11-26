layout(location=0) in vec3 vertex_position;
layout(location=1) in vec2 vertex_uv;
layout(location=2) in vec4 vertex_color;     // its light, its alpha its glow
const float GLOW = 3.0;   // a vertex's glow (its colour's alpha, doc/plans/hdr.md): up to this much past white

uniform mat4 mvp_matrix;
out float fog_level;
out vec2 uv;
out vec3 vertex_color_frag;

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    fog_level = gl_Position.z * gl_Position.z;
    uv = vertex_uv;
    vertex_color_frag = max(vertex_color.rgb, vec3(vertex_color.a)) * (1.0 + vertex_color.a * GLOW);
}
