out vec4 frag_color;
in float fog_level;
uniform vec4 fog_data;
uniform vec4 color;

void main() {
    frag_color = color;
    float fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
    frag_color.xyz = mix(frag_color.xyz, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend);
}
