in vec2 uv;
uniform float brightness;
uniform float alpha;
uniform sampler2D zexture;
uniform float blur;
uniform float vignette;
uniform float tint;     // under water, 0 to 1: deep blue
uniform float lids;     // the eyes closing, 0 open to 1 shut
out vec4 frag_color;    // alpha 1: the ui draws it blended (a vec3 left alpha undefined, the view black)
#ifdef zox_outlines
// outlines (doc/plans/outlines.md): the render camera's depth, and its mask (outlined models' id and own depth)
uniform sampler2D depth_texture;
precision highp usampler2D;
uniform usampler2D mask_texture;
uniform vec4 outline_data;      // on, the camera's near and far, a line's width in pixels times its depth (a block
                                //  texture's texel, as blocks' baked outlines)
const float OUTLINE_GAP_MIN = 0.0625;   // 4 model voxels: smaller steps in depth draw no line
const float OUTLINE_GAP_FULL = 0.375;   // 24: a full line, steps between are fainter and thinner
const vec2 OUTLINE_LIGHT_AWAY = vec2(0.7071, -0.7071);   // the light from the top left: thicker to the bottom right

float outline_linear(float depth) {
    float n = outline_data.y;
    float f = outline_data.z;
    float z = depth * 2.0 - 1.0;
    return 2.0 * n * f / (f + n - z * (f - n));
}

// a model's linear depth at a pixel, 0 where none drew: the nearer of the depth buffer's and the mask's (a see
//  through shell writes no depth, only its mask). Outlined models draw after terrain, nothing solid hides them after
float outline_model(vec2 at, float scene) {
    uvec2 mask = texelFetch(mask_texture, ivec2(at * vec2(textureSize(mask_texture, 0))), 0).xy;
    if (mask.x == 0u) {
        return 0.0;
    }
    return outline_linear(min(scene, uintBitsToFloat(mask.y)));
}

// the line from any outlined model nearer than this pixel, in 8 directions: its width by its depth, the gap and
//  the light's side, near black (a dark shade of the model's colour), as the blocks' baked outlines
vec3 outline(vec3 color) {
    vec2 texel = 1.0 / vec2(textureSize(depth_texture, 0));
    float scene = texture(depth_texture, uv).r;
    float model = outline_model(uv, scene);
    float here = model > 0.0 ? model : outline_linear(scene);
    float best = 0.0;
    vec3 line = vec3(0.0);
    for (int i = 0; i < 8; i++) {
        float angle = float(i) * 0.7853982;
        vec2 direction = vec2(cos(angle), sin(angle));
        float side = 1.0 + 0.2 * dot(direction, OUTLINE_LIGHT_AWAY);
        for (int r = 1; r <= 8; r++) {
            vec2 at = uv + direction * float(r) * texel;
            float there = outline_model(at, texture(depth_texture, at).r);
            float gap = here - there;
            if (there <= 0.0 || gap < OUTLINE_GAP_MIN) {
                continue;
            }
            float g = smoothstep(OUTLINE_GAP_MIN, OUTLINE_GAP_FULL, gap);
            float width = clamp(outline_data.w / there, 1.0, 5.0) * (0.6 + 0.4 * g) * side;
            float coverage = (0.5 + 0.5 * g) * (1.0 - smoothstep(width - 0.5, width + 0.5, float(r)));
            if (coverage > best) {
                best = coverage;
                line = texture(zexture, at).rgb * 0.12;
            }
        }
    }
    return mix(color, line, best);
}
#endif

uniform float tonemap;  // colours past 1 (a camera's half float target) back to the screen's: 0 clamp, 1 soft shoulder
                        //  past 0.8, 2 filmic (doc/plans/hdr.md)
const float TONEMAP_KNEE = 0.8;

vec3 tone(vec3 c) {
    c = max(c, vec3(0.0));
    if (tonemap < 0.5) {
        return min(c, vec3(1.0));
    }
    if (tonemap < 1.5) {
        // on the brightest channel, so a hue keeps its ratios as it nears white
        float m = max(c.r, max(c.g, c.b));
        if (m <= TONEMAP_KNEE) {
            return c;
        }
        float t = TONEMAP_KNEE + (1.0 - TONEMAP_KNEE) * (1.0 - exp(-(m - TONEMAP_KNEE) / (1.0 - TONEMAP_KNEE)));
        return c * (t / m);
    }
    // the aces fit (Narkowicz 2015)
    return clamp((c * (2.51 * c + 0.03)) / (c * (2.43 * c + 0.59) + 0.14), 0.0, 1.0);
}

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
    vec3 color;
    vec3 base = texture(zexture, uv).rgb;
#ifdef zox_outlines
    if (outline_data.x > 0.0) {
        base = outline(base);
    }
#endif
    // Blur
    if (blur > 0.0) {
        vec3 blurred = gaussian_blur_rgb(zexture, uv, BLUR_RADIUS);
        color = mix(base, blurred, blur);
    } else {
        color = base;
    }
    color = tone(color);

    // Noise
    float noise = random(uv);
    color = mix(
        color,
        vec3(noise),
        NOISE_STRENGTH);

    // Under water: reds and greens soaked up a little, the view blue (light: it multiplies the dark)
    color = mix(color, color * vec3(0.6, 0.82, 1.05) + vec3(0.0, 0.03, 0.07), tint);

    // Vignette
    float vignette_smoothness = 0.8;
    float vignette_mask = smoothstep(1.0 - vignette_smoothness, 1.0, length(uv - vec2(0.5, 0.5)));
    color *= 1.0 - vignette_mask * (vignette);

    // Eyelids: dark from the top (0.6 of the way) and the bottom (0.4), the middle first (as xr/fun/eye_render.c)
    if (lids > 0.0) {
        float u = uv.x * 2.0 - 1.0;
        float bulge = 1.0 + 0.35 * (1.0 - u * u);
        float top = min(1.0, 0.6 * lids * bulge);
        float bottom = min(1.0, 0.4 * lids * bulge);
        float edge = 0.015;
        float lid = max(smoothstep(1.0 - top - edge, 1.0 - top, uv.y), 1.0 - smoothstep(bottom, bottom + edge, uv.y));
        color *= lids >= 1.0 ? 0.0 : 1.0 - lid;
    }

    // Apply Properties
    color *= brightness;
    frag_color = vec4(color, 1.0);
}
