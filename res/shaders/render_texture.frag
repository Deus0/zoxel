in vec2 uv;
out vec4 color;
uniform float brightness;
uniform float alpha;
uniform sampler2D zexture;
uniform float blur;
uniform float vignette;

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

// simple blur
vec4 box_blur(sampler2D image, vec2 uv, float radius) {
    vec4 sum = vec4(0.0);
    float offset = radius / 512.0; // tweak depending on resolution
    sum += texture(image, uv + vec2(-offset, -offset));
    sum += texture(image, uv + vec2( 0.0,    -offset));
    sum += texture(image, uv + vec2( offset, -offset));
    sum += texture(image, uv + vec2(-offset,  0.0));
    sum += texture(image, uv);
    sum += texture(image, uv + vec2( offset,  0.0));
    sum += texture(image, uv + vec2(-offset,  offset));
    sum += texture(image, uv + vec2( 0.0,     offset));
    sum += texture(image, uv + vec2( offset,  offset));
    return sum / 9.0;
}

// Gaussian blur (9-tap)
vec4 gaussian_blur(sampler2D image, vec2 uv, float radius) {
    float offset = radius / 512.0; // tweak for resolution
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
    vec4 sum = vec4(0.0);
    float weightSum = 0.0;
    for (int i = 0; i < 9; i++) {
        vec4 s = texture(image, uv + offsets[i]);
        sum += s * GAUSS_KERNEL[i];
        weightSum += GAUSS_KERNEL[i];
    }
    return sum / weightSum;
}

void main() {
    vec4 base = texture(zexture, uv);
    // Blur
    if (blur > 0.0) {
        vec4 blurred = gaussian_blur(zexture, uv, BLUR_RADIUS);
        color = mix(base, blurred, blur);
    } else {
        color = base;
    }

    // Noise
    float noise = random(uv);
    color = mix(color, vec4(noise, noise, noise, 1.0), NOISE_STRENGTH);

    //vec4 blurred = gaussian_blur(zexture, uv, BLUR_RADIUS);
    //color = mix(base, blurred, 0.9);
    // === vignette ===
    // float vignette_intensity = 0.7;     // change to 0.9 for dialogue
    float vignette_smoothness = 0.8;
    float vignette_mask = smoothstep(1.0 - vignette_smoothness, 1.0, length(uv - vec2(0.5, 0.5)));
    color.rgb *= 1.0 - vignette_mask * (vignette);

    // Apply Properties
    color.a = base.a;
    color.rgb *= brightness;
    color.a *= alpha;
}
