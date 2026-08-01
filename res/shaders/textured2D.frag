uniform float brightness;
uniform float alpha;
uniform sampler2D zexture;
in vec2 uv;
out vec4 color;

void main() {
    color = texture(zexture, uv);
    color.rgb *= brightness;
    color.a *= alpha;
}
