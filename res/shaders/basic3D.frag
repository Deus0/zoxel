out vec4 frag_color;
uniform vec4 color;

void main() {
    frag_color = vec4(zox_fog(color.xyz), color.w);
}
