out vec4 color;
in vec2 uv;
uniform float brightness;
uniform float alpha;
uniform sampler2D zexture;

void main() {
    color = texture(zexture, uv);
    color.rgb *= brightness;
    color.a *= alpha;
}
