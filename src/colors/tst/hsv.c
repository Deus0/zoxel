// tst: colors_hsv, pure logic for src/colors/fun/hsv_rgb.c (no world, no gpu)
//  - saturating past 100 keeps a pure colour pure: red stays (255, 0, 0); a channel went below 0 and its float to
//    byte was undefined, gcc wrapped it (green 154), clang (web, -O3) dropped stats_realm_spawn_system for it
//  - the realm's stat colours (saturate 1.4, darken 0.4, gam/zoxel/sys/stats.c) stay within their darkened value
//  - colours go to hsv and back within a step
//  - run: bash bsh/tst.sh colors_hsv

byte zox_tst_colors_ready(ecs* world) {
    (void) world;
    return 1;
}

static inline int zox_tst_colors_error(const color_rgb a, const color_rgb b) {
    const int r = abs((int) a.r - (int) b.r);
    const int g = abs((int) a.g - (int) b.g);
    const int bl = abs((int) a.b - (int) b.b);
    return r > g ? (r > bl ? r : bl) : (g > bl ? g : bl);
}

void zox_tst_colors_hsv_check(ecs* world) {
    (void) world;
    const color_rgb red = color_rgb_saturate(color_rgb_red, 1.4f);
    zox_tst_check(red.r == 255 && red.g == 0 && red.b == 0, "red saturated 1.4 is [%i %i %i], wants [255 0 0]",
        red.r, red.g, red.b);
    // hsv out of range: saturation 200, value 150, still a colour (channels clamped, not wrapped)
    const color_rgb over = hsv_to_color_rgb((float3) { 0, 200, 150 });
    zox_tst_check(over.r == 255 && over.g == 0 && over.b == 0, "hsv [0 200 150] is [%i %i %i], wants [255 0 0]",
        over.r, over.g, over.b);
    // the realm's stat colours: darkened to 0.4, no channel over 0.4 of 255 (a wrapped one was)
    const color_rgb stat_colors[] = { color_rgb_purple, color_rgb_red, color_rgb_yellow, color_rgb_cyan };
    int worst = 0;
    for (int i = 0; i < 4; i++) {
        const color_rgb c = color_rgb_darken(color_rgb_saturate(stat_colors[i], 1.4f), 0.4f);
        const int top = c.r > c.g ? (c.r > c.b ? c.r : c.b) : (c.g > c.b ? c.g : c.b);
        if (top > worst) {
            worst = top;
        }
    }
    zox_tst_check(worst <= 103, "a stat colour's brightest channel is [%i], darkened to 0.4 wants 103 at most", worst);
    // round trip, every 17th step of each channel
    int worst_trip = 0;
    for (int r = 0; r < 256; r += 17) {
        for (int g = 0; g < 256; g += 17) {
            for (int b = 0; b < 256; b += 17) {
                const color_rgb c = { (byte) r, (byte) g, (byte) b };
                const int error = zox_tst_colors_error(c, hsv_to_color_rgb(color_rgb_to_hsv(c)));
                if (error > worst_trip) {
                    worst_trip = error;
                }
            }
        }
    }
    zox_tst_check(worst_trip <= 1, "a colour to hsv and back is off by [%i], wants 1 at most", worst_trip);
}

void zox_tsts_colors(ecs* world) {
    (void) world;
    zox_tst_add("colors_hsv", zox_tst_colors_ready, zox_tst_colors_hsv_check, 30);
}
