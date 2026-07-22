out vec4 color;
in vec2 uv;
uniform sampler2D tex;
uniform float brightness;
uniform float alpha;

void main() {
    color = texture(tex, uv);
    color.rgb *= brightness;
    color.a *= alpha;
}
