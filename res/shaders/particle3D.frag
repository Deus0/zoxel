precision highp float;

out vec4 color_output;
uniform vec4 fog_data;
in float fog_level;
in vec4 frag_color;

void main() {
    float fog_blend = 1.0 - exp2(-fog_data.w * fog_level * fog_level);
    color_output = vec4(mix(frag_color.xyz, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend), frag_color.w);
}
