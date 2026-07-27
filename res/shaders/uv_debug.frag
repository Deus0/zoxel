out vec4 color;
in vec2 uv;
uniform float brightness;
uniform float alpha;

void main() {
    color = vec4(uv.x, uv.y, 0.0, 1.0);
    color.rgb *= brightness;
    color.a *= alpha;
}
