out vec4 color_output;
in vec4 frag_color;

void main() {
    vec3 color_blend = zox_fog(frag_color.xyz);
    color_output = vec4(color_blend, frag_color.w);
    // color_output = vec4(1.0, 0, 0, 0.9);
}
