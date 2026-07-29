in vec3 vertex_color_frag;
in float fog_blend;
out vec3 frag_color;
uniform vec4 fog_data;
uniform float brightness;

void main() {
    frag_color = vertex_color_frag * brightness;
    frag_color = mix(frag_color, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend);
}
