precision mediump float;

out lowp vec4 color;
uniform lowp float brightness;

void main() {
    color = vec4(0.5, 0.65, 0.15, 0) * brightness;
}
