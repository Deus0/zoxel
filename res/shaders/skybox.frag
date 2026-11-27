// the sky, by direction (the skybox cube's mesh_pos normalized: no corners or faces show)
//  - the biome's colours from the horizon up, lit by the sky's light (view.glsl's sky_tint: dark and blue at night)
//  - the sun a square (blocky, as the world), where sun_direction points, a glow round it, the horizon warm on its
//    side at sunrise and sunset; stars at night (doc/plans/day_night.md)
out vec3 frag_color;
in vec3 mesh_pos;
uniform vec4 color;
uniform float time;
uniform vec3 sky_top_color;
uniform vec3 sky_bottom_color;
const float SKY_LIFT = 0.35;            // lighter overhead (as the old sky's brightening towards the top)
const float SUN_SIZE = 0.045;           // the sun square's half width, across its direction
const float SUN_BRIGHT = 2.5;           // high up, past white: bloom glows round it (doc/plans/hdr.md)
const float SUN_BRIGHT_LOW = 1.1;       // on the horizon, dimmer: its sunset colour, not clipped to white

// a hash without sin: fract(sin(big)) lost its precision as time grew, the sky's noise turned to streaks
float rand(vec3 p) {
    vec3 p3 = fract(p * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

// the animated noise, by direction (cells of about 0.1 degrees), shifted each frame, its input kept small
//  - it was per pixel on screen (gl_FragCoord): in a headset it stayed on the view, the sky seemed to turn with
//      the head, and each eye's differed
float sky_noise(vec3 direction) {
    return rand(floor(direction * 600.0) + floor(fract(time * 0.37) * 1024.0));
}

// 1 inside the sun's square, seen along direction
float sun_square(vec3 direction, vec3 sun) {
    float facing = dot(direction, sun);
    if (facing <= 0.0) {
        return 0.0;
    }
    vec3 side = normalize(abs(sun.y) > 0.99 ? cross(sun, vec3(1.0, 0.0, 0.0)) : cross(sun, vec3(0.0, 1.0, 0.0)));
    vec3 up = cross(side, sun);
    vec2 across = vec2(dot(direction, side), dot(direction, up)) / facing;
    return step(max(abs(across.x), abs(across.y)), SUN_SIZE);
}

void main() {
    vec3 direction = normalize(mesh_pos);
    vec3 sun = normalize(sun_direction.xyz);
    float height = direction.y;
    // the biome's colours: the horizon's up to the top's, lighter overhead, in the sky's light; at the horizon
    //  it's the fog's colour (view.glsl's zox_horizon), the terrain fades into it
    vec3 sky = mix(sky_bottom_color, sky_top_color, smoothstep(0.0, 0.6, height));
    sky += vec3(SKY_LIFT) * smoothstep(0.2, 1.0, height);
    sky = sky * sky_tint.xyz + zox_sun_glow(direction);
    // stars: a few direction cells lit, as the sky darkens, above the horizon
    float night = 1.0 - smoothstep(0.15, 0.45, dot(sky_tint.xyz, vec3(0.333)));
    float star = step(0.9965, rand(floor(direction * 140.0) + 17.0));
    sky += vec3(0.8, 0.85, 1.0) * star * night * smoothstep(0.0, 0.15, height);
    // grain, as bright as the sky
    sky += (sky_noise(direction) * 0.2 - 0.1) * dot(sky_tint.xyz, vec3(0.333));
    // the sun over it all, its colour (the realm's sunset low down)
    float bright = mix(SUN_BRIGHT_LOW, SUN_BRIGHT, smoothstep(0.0, 0.3, sun.y));
    sky = mix(sky, sun_color.xyz * bright, sun_square(direction, sun) * zox_sun_up());
    // a thick fog (under water, or the view fading to dark) covers the sky too; the open air's thin fog leaves it
    frag_color = mix(sky, fog_data.xyz, zox_fog_thick());
}
