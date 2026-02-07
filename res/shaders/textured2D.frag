precision highp float;

out vec4 color;
in vec2 uv;
uniform sampler2D tex;
uniform float brightness;
uniform float alpha;

void main() {
    color = texture(tex, uv) * brightness;
    color.w *= alpha;
    if (alpha < -0.99) color.w = 1.0;
}
