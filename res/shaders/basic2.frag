precision mediump float;

out vec4 color;
uniform float brightness;

void main() {
    color = vec4(0.5, 0.65, 0.15, 0) * brightness;
}
