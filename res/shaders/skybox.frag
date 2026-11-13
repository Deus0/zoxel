out vec3 frag_color;
in vec3 mesh_pos;
in vec3 sky_dir;     // its direction in the world (mesh_pos turns with the camera)
in float fog_level;
// uniform float brightness;
uniform vec4 color;
uniform vec4 fog_data;
uniform float time;
uniform vec3 sky_top_color;
uniform vec3 sky_bottom_color;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

vec3 sky_gradient(vec3 color) {
    if (mesh_pos.y > 0.1) {
        float height_power = (mesh_pos.y - 0.1) * (1.0 - abs(max(mesh_pos.x, mesh_pos.z))) * 1.1;
        color.x += height_power;
        color.y += height_power;
        color.z += height_power;
        if (mesh_pos.x >= -0.1 && mesh_pos.x <= 0.1 && mesh_pos.z >= -0.1 && mesh_pos.z <= 0.1) {
            float to_mid_power = (1.0 - 10.0 * max(abs(mesh_pos.x), abs(mesh_pos.z)));
            color.x += to_mid_power;
            color.y += to_mid_power;
            color.z += to_mid_power;
            float noise2 = rand(time * mesh_pos.xy);
            color -= vec3(noise2) * 1.4;
        }
    }
    return color;
}

// Clouds: a flat layer high above, blocky like voxels, drifting with the wind, tinted by the sky
//  - a sky pixel's direction meets the layer at dir.xz / dir.y, snapped to a grid (blocks)
//  - two octaves of value noise, cut off for gaps of clear sky, fading at the horizon
const float cloud_scale = 2.5;          // cells across the layer a unit of direction
const float cloud_cells = 6.0;          // grid snaps a cell, the blockiness
const float cloud_cover = 0.5;          // how much is cloud, higher is clearer
const vec2 cloud_wind = vec2(0.012, 0.006);

// a hash without sin (the sin * 43758 one breaks at gles mediump), coordinates kept small
float cloud_hash(vec2 p) {
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

float cloud_noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = cloud_hash(i);
    float b = cloud_hash(i + vec2(1.0, 0.0));
    float c = cloud_hash(i + vec2(0.0, 1.0));
    float d = cloud_hash(i + vec2(1.0, 1.0));
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// 0 clear to 1 thick cloud, for a direction
float clouds(vec3 dir) {
    if (dir.y <= 0.02) {
        return 0.0;
    }
    vec2 layer = dir.xz / dir.y * cloud_scale + time * cloud_wind;
    // wrapped, so numbers stay small at mediump however long the game runs
    layer = mod(layer, 64.0);
    vec2 cell = floor(layer * cloud_cells) / cloud_cells;
    float n = cloud_noise(cell * 3.0) * 0.65 + cloud_noise(cell * 7.0 + 17.0) * 0.35;
    float cloud = smoothstep(cloud_cover, cloud_cover + 0.08, n);
    // thin out at the horizon, where cells squash into streaks
    return cloud * smoothstep(0.02, 0.25, dir.y);
}

void main() {
    // Main vertical sky gradient.
    // Keeps the horizon close to the terrain colour while becoming
    // substantially lighter toward the top.
    float gradient = smoothstep(0.0, 1.0, mesh_pos.y);
    frag_color = mix(
        sky_bottom_color,
        sky_top_color,
        gradient * 0.7
    );

    // Extra lift toward the upper sky so the gradient is actually visible.
    float upper_gradient = smoothstep(0.35, 1.0, mesh_pos.y);
    frag_color += vec3(upper_gradient * 0.08);

    // Existing sky/sun shaping.
    frag_color = sky_gradient(frag_color);

    // Small animated variation.
    float noise = rand(time * mesh_pos.xy);
    frag_color += vec3((noise * 0.2) - 0.1);

    // Clouds over it, white tinted by the sky above
    float cloud = clouds(normalize(sky_dir));
    vec3 cloud_color = mix(vec3(0.96), sky_top_color, 0.15);
    frag_color = mix(frag_color, cloud_color, cloud * 0.9);
}
