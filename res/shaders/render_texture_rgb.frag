in vec2 uv;
uniform float brightness;
uniform float alpha;
uniform sampler2D zexture;
uniform float blur;
uniform float vignette;
out vec3 frag_color;

// === constants ===
const float BLUR_RADIUS = 3.0;
const float BLUR_SCALE = 512.0;
const float NOISE_STRENGTH = 0.1;

const float GAUSS_KERNEL[9] = float[](
    1.0, 2.0, 1.0,
    2.0, 4.0, 2.0,
    1.0, 2.0, 1.0
);

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec3 gaussian_blur_rgb(sampler2D image, vec2 uv, float radius) {
    float offset = radius / BLUR_SCALE; // tweak for resolution
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0,     offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0),    // center-left
        vec2( 0.0,     0.0),    // center
        vec2( offset,  0.0),    // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0,    -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
    );
    vec3 sum = vec3(0.0);
    float weightSum = 0.0;
    for (int i = 0; i < 9; i++) {
        vec3 s = texture(image, uv + offsets[i]).rgb;
        sum += s * GAUSS_KERNEL[i];
        weightSum += GAUSS_KERNEL[i];
    }
    return sum / weightSum;
}

void main() {
    vec3 base = texture(zexture, uv).rgb;
    // Blur
    if (blur > 0.0) {
        vec3 blurred = gaussian_blur_rgb(zexture, uv, BLUR_RADIUS);
        frag_color = mix(base, blurred, blur);
    } else {
        frag_color = base;
    }

    // Noise
    float noise = random(uv);
    frag_color = mix(
        frag_color,
        vec3(noise),
        NOISE_STRENGTH);

    // Vignette
    float vignette_smoothness = 0.8;
    float vignette_mask = smoothstep(1.0 - vignette_smoothness, 1.0, length(uv - vec2(0.5, 0.5)));
    frag_color *= 1.0 - vignette_mask * (vignette);

    // Apply Properties
    frag_color *= brightness;
}
