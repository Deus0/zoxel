out vec4 frag_color;
in vec3 vertex_color_frag;
in float fog_blend;
uniform vec4 fog_data;
uniform float brightness;

void main() {
    frag_color.rgb = vertex_color_frag * brightness;
    // frag_color = mix(frag_color, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend);
    frag_color.rgb = mix(frag_color.rgb, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend);
    frag_color.a = 1.0;
}
