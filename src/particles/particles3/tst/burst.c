// tst: particle_burst, exact counts, palette and fallback colors, configured motion and life.
static void zox_tst_particle_burst_check(ecs* world) {
    const entity model = zox_new();
    const color palette[] = { { 14, 52, 122, 255 }, { 201, 113, 8, 255 } };
    ColorRGBs colors = { 0 };
    initialize_ColorRGBs(&colors, 2);
    for (int i = 0; i < 2; i++) {
        colors.value[i] = (color_rgb) { palette[i].r, palette[i].g, palette[i].b };
    }
    zox_set_ptr(model, ColorRGBs, colors);
    particle3_burst burst = {
        .position = { 30, 40, 50 }, .velocity = { 1, -2, 3 }, .spread = { 0.25f, 0.5f, 0 },
        .gravity = { 0, -4, 0 }, .life = 0.75f, .fallback = { 33, 44, 55, 123 }, .count = 32
    };
    const float3 palette_position = burst.position;
    spawn_particle3_burst(world, model, &burst, 666);
    burst.position.x += 10;
    burst.count = 5;
    spawn_particle3_burst(world, 0, &burst, 902);
    burst.count = 0;
    const int total = zox_count_types(Particle3D);
    spawn_particle3_burst(world, model, &burst, 666);
    zox_tst_check(zox_count_types(Particle3D) == total, "an empty burst spawned particles");
    int palette_count = 0, fallback_count = 0, palette_hits[2] = { 0 }, wrong = 0;
    zox_each_begin(Particle3D);
        const float3 position = zox_getv(e, Position3D);
        const byte from_palette = float3_equals(position, palette_position);
        if (!from_palette && !float3_equals(position, burst.position)) {
            continue;
        }
        const color c = zox_getv(e, Color);
        if (from_palette) {
            const byte first = color_equals(c, palette[0]);
            const byte second = color_equals(c, palette[1]);
            palette_hits[0] += first;
            palette_hits[1] += second;
            palette_count++;
            wrong += !first && !second;
        } else {
            fallback_count++;
            wrong += !color_equals(c, burst.fallback);
        }
        const float3 velocity = float3_sub(zox_getv(e, Velocity3D), burst.velocity);
        wrong += fabsf(velocity.x) > burst.spread.x || fabsf(velocity.y) > burst.spread.y || velocity.z != 0;
        wrong += !float3_equals(zox_getv(e, Gravity3D), burst.gravity);
        wrong += zox_getv(e, DestroyInTime) != burst.life || zox_getv(e, FadeOutTime) != burst.life;
    zox_each_end();
    zox_tst_check(palette_count == 32 && fallback_count == 5, "burst counts were [%i] palette and [%i] fallback", palette_count, fallback_count);
    zox_tst_check(palette_hits[0] && palette_hits[1], "fixed seed did not use both palette colors");
    zox_tst_check(!wrong, "burst particles had [%i] wrong colors, velocities, gravity or lifetimes", wrong);
}
