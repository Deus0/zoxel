uniform float brightness;
uniform float alpha_cutoff;    // 0 (see through pixels discarded), 0.5 writing a 3D ui's depth (only its solid parts)
uniform sampler2D zexture;
in vec3 vertex_color_frag;
in vec2 uv;
out vec4 frag_color;

void main() {
    vec4 texture_color = texture(zexture, uv);
    if (texture_color.a <= alpha_cutoff) {
        discard;
    }
    vec3 mix_color = texture_color.xyz * vertex_color_frag * brightness;
    mix_color = zox_fog(mix_color);
    frag_color = vec4(mix_color, texture_color.w);
    // Debug
    // frag_color = vec4(1, 0, 0, 1);
}
