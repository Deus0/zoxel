layout(location=0) in vec3 vertex_position;
layout(location=1) in vec4 vertex_color;     // its colour, its alpha its glow
const float GLOW = 3.0;   // a vertex's glow (its colour's alpha, doc/plans/hdr.md): up to this much past white

uniform mat4 mvp_matrix;
uniform vec4 fog_data;
out vec3 vertex_color_frag;
out float fog_blend;

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    vertex_color_frag = vertex_color.rgb * (1.0 + vertex_color.a * GLOW);
    float fog_level = gl_Position.z * gl_Position.z;
    fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
}
