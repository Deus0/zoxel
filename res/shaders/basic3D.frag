out vec3 frag_color;
in float fog_level;
uniform vec4 fog_data;
uniform float brightness;

void main() {
    frag_color = vec3(brightness, brightness / 3.0, brightness / 3.0);
    float fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
    frag_color = mix(frag_color, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend);
}
