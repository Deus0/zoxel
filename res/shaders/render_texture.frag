precision mediump float;

out vec4 color;
uniform sampler2D tex;
in vec2 uv;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    color = texture(tex, uv);
    float noise = random(uv);
    float vignette = smoothstep(0.8, 0.2, distance(uv, vec2(0.5)));
    color.rgb *= vignette;
    color = mix(color, vec4(noise, noise, noise, 1.0), 0.1);
}
