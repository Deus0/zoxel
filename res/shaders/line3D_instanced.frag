out vec4 frag_color;
in vec4 vertex_color;

void main() {
    frag_color = mix(vertex_color, vec4(zox_fog_color(), 0.0), zox_fog_amount());    // faded out
    gl_FragDepth = gl_FragCoord.z - 0.0002;
}
