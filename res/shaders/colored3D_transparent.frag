in vec3 vertex_color_frag;
uniform float brightness;
out vec4 frag_color;

void main() {
    vec3 frag_color3 = vertex_color_frag * brightness;
    frag_color3 = zox_fog(frag_color3);
    frag_color = vec4(frag_color3.x, frag_color3.y, frag_color3.z, 0.4);
}
