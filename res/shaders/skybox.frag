out vec3 frag_color;
in vec3 mesh_pos;
in float fog_level;
// uniform float brightness;
uniform vec4 color;
uniform float time;
uniform vec3 sky_top_color;
uniform vec3 sky_bottom_color;

// a hash without sin: fract(sin(big)) lost its precision as time grew, the sky's noise turned to streaks
float rand(vec3 p) {
    vec3 p3 = fract(p * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

// the animated noise, by direction (cells of about 0.1 degrees), shifted each frame, its input kept small
//  - it was per pixel on screen (gl_FragCoord): in a headset it stayed on the view, the sky seemed to turn with
//      the head, and each eye's differed
//  - mesh_pos.xy was flat on the cube's x faces (only y changes there): rows of one value, streaks
float sky_noise() {
    return rand(floor(normalize(mesh_pos) * 600.0) + floor(fract(time * 0.37) * 1024.0));
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
            float noise2 = sky_noise();
            color -= vec3(noise2) * 1.4;
        }
    }
    return color;
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
    float noise = sky_noise();
    frag_color += vec3((noise * 0.2) - 0.1);

    // a thick fog (under water, or the view fading to dark) covers the sky too; the open air's thin fog leaves it
    frag_color = mix(frag_color, fog_data.xyz, clamp((fog_data.w - 0.01) * 25.0, 0.0, 1.0));
}
