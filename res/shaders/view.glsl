// the camera drawing now, added before every shader as it loads (src/rendering/shd/fun/view.c, doc/howto/shaders.md)
//  - src/rendering/dat/camera_renderer.c's RenderView, uploaded once a camera
//  - mediump: a block's members must have one precision in both stages (gles), the fragment's (shd/fun/precision.c);
//    highp ones mixed into a mediump fragment's maths drew nothing on vulkan (lavapipe, tst body_views)
//  - the fog and the sky's horizon are here, one place to tune them: every 3D shader ends with zox_fog
layout(std140) uniform RenderView {
    mediump vec4 fog_data;      // the fog's colour (xyz, the sky's at the horizon, darkened by its light) and density (w)
    mediump vec4 sky_tint;      // the sky's light on the world (xyz): its suns summed, at least the night's
    mediump vec4 sun_direction; // towards the brightest sun (xyz), normalized
    mediump vec4 sun_color;     // its colour (xyz): the realm's sunset low down, its own high up
    mediump vec4 camera_position;   // the camera's place (xyz)
    mediump vec4 frag_viewport; // gl_FragCoord back to the viewport: (frag - xy) / zw is 0 to 1 across it
    mediump mat4 clip_to_view;  // clip space back to a point's offset from the camera, world axes
};

const float FOG_BASE = 16.0;    // metres up: below it the fog is its thickest
const float FOG_HEIGHT = 20.0;  // above that it halves each this much higher: hill tops and clouds fade less (clouds
                                //  at 160m about 1/20th, as their own lighter fog was)
const float FOG_HAZE = 0.1;     // a little even close by


// a lit thing's light from its sky's and blocks' (0 to 1, src/lights: two trees): the brighter of the sky's, in its
//  light now (dark at night), and blocks' (glow stones, lamps: as bright at night)
vec3 zox_light(float sky, float block) {
    return max(sky * sky_tint.xyz, vec3(block));
}

// a lit colour's light, glow its glow (0 to 1, its colour's alpha): a glowing colour keeps its own, at night too
vec3 zox_lit(float sky, float block, float glow) {
    return mix(zox_light(sky, block), vec3(1.0), clamp(glow, 0.0, 1.0));
}

// a thick fog (under water, the view fading to dark): it covers the sky, no sun glows through it
float zox_fog_thick() {
    return clamp((fog_data.w - 0.01) * 25.0, 0.0, 1.0);
}

// how much the sun is up: gone under the ground
float zox_sun_up() {
    return smoothstep(-0.12, 0.02, sun_direction.y);
}

// the sky's glow round the sun, and along the horizon towards it when it's low (sunrise, sunset), seen along
//  direction: in the sky and the fog both, so the terrain in front of it fades into it
vec3 zox_sun_glow(vec3 direction) {
    vec3 sun = sun_direction.xyz;
    float facing = max(dot(direction, sun), 0.0);
    float low = 1.0 - smoothstep(0.0, 0.35, abs(sun.y));
    float horizon = 1.0 - smoothstep(0.0, 0.45, abs(direction.y));
    float glow = low * horizon * pow(facing, 3.0) * 0.5 + pow(facing, 48.0) * 0.35;
    return sun_color.xyz * glow * zox_sun_up() * (1.0 - zox_fog_thick());
}

// the sky at the horizon, seen along direction: what the fog fades into
vec3 zox_horizon(vec3 direction) {
    return fog_data.xyz + zox_sun_glow(direction);
}

#ifdef zox_fragment
// this pixel's point from the camera, world axes: its depth back through the view (highp: far depths)
highp vec3 zox_view_offset() {
    highp vec2 across = (gl_FragCoord.xy - frag_viewport.xy) / frag_viewport.zw * 2.0 - 1.0;
    highp vec4 point = clip_to_view * vec4(across, gl_FragCoord.z * 2.0 - 1.0, 1.0);
    return point.xyz / point.w;
}

// how much fog covers this pixel: by its distance, thinner the higher it is (not under water)
float zox_fog_amount() {
    highp vec3 offset = zox_view_offset();
    highp float distance2 = dot(offset, offset);
    float height = camera_position.y + offset.y * 0.5 - FOG_BASE;
    float thinning = mix(exp2(-max(height, 0.0) / FOG_HEIGHT), 1.0, zox_fog_thick());
    highp float density = fog_data.w * thinning;
    return min(1.0, 1.0 + FOG_HAZE - exp2(-density * distance2));
}

// this pixel's fog colour: the sky's horizon behind it
vec3 zox_fog_color() {
    return zox_horizon(normalize(zox_view_offset()));
}

// a colour fogged: the end of every 3D fragment shader
vec3 zox_fog(vec3 color) {
    return mix(color, zox_fog_color(), zox_fog_amount());
}
#endif
