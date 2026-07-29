uniform vec4 fog_data;
uniform float brightness;
uniform sampler2D zexture;
in float fog_level;
in vec3 vertex_color_frag;
in vec2 uv;
out vec4 frag_color;

void main() {
    vec4 texture_color = texture(zexture, uv);
    if (texture_color.a <= 0.0) {
        discard;
    }
    vec3 mix_color = texture_color.xyz * vertex_color_frag * brightness;
    float fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));
    mix_color = mix(mix_color, vec3(fog_data.x, fog_data.y, fog_data.z), fog_blend);
    frag_color = vec4(mix_color, texture_color.w);
    // Debug
    // frag_color = vec4(1, 0, 0, 1);
}
