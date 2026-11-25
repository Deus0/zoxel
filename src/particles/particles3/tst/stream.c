// tst: particle_stream, the prefab child follows its parent and emits per second at two update rates.
static void zox_tst_particle_stream_ticks(ecs* world, int steps, double time) {
    const double old_time = zox_current_time;
    const double old_delta = zox_delta_time;
    zox_delta_time = 1.0 / steps;
    for (int i = 1; i <= steps; i++) {
        zox_current_time = time + i * zox_delta_time;
        zox_run(world, zox_id(particle3_stream_system));
        zox_run(world, zox_id(particle3d_emit_system));
    }
    zox_current_time = old_time;
    zox_delta_time = old_delta;
}

static void zox_tst_particle_stream_check(ecs* world) {
    const entity prefab = zox_new();
    zox_make_prefab(prefab);
    zox_setv(prefab, Position3D, float3_zero);
    zox_setv(prefab, Rotation3D, quaternion_identity);
    const float3 bounds = { 0.125f, 0.25f, 0.5f };
    const float life = 0.6f;
    const int rate = 24;
    const float rise = 2;
    add_prefab_particle3_streamer(world, prefab, rate, life, bounds, rise);
    const entity parent = zox_ins(world, prefab);
    const entity streamer = get_particle3_streamer(world, parent);
    zox_tst_check(zox_valid(streamer) && zox_get_parent(world, streamer) == parent, "the instance did not copy its streamer child");
    if (!zox_valid(streamer)) {
        return;
    }
    const float3 centres[] = { { 100, 12, -50 }, { 120, -5, 10 } };
    const color parent_colors[] = { { 21, 140, 91, 160 }, { 160, 34, 91, 210 } };
    const int steps[] = { 16, 128 };
    const int before = zox_count_types(Particle3D);
    for (int pass = 0; pass < 2; pass++) {
        zox_setv(parent, Position3D, centres[pass]);
        zox_setv(parent, Color, parent_colors[pass]);
        zox_run(world, zox_id(transform3_children_system));
        zox_tst_check(float3_equals(zox_getv(streamer, Position3D), centres[pass]), "the emitter did not follow its parent");
        zox_tst_particle_stream_ticks(world, steps[pass], 10.03125 + pass);
        zox_tst_check(zox_count_types(Particle3D) - before == (pass + 1) * rate,
            "the [%i] Hz stream did not emit [%i] particles in one second", steps[pass], rate);
    }
    int counts[2] = { 0 }, wrong = 0;
    zox_each_begin(Particle3D);
        const float3 position = zox_getv(e, Position3D);
        const int pass = position.x < 110 ? 0 : 1;
        const float3 offset = float3_sub(position, centres[pass]);
        if (fabsf(offset.x) > bounds.x || fabsf(offset.y) > bounds.y || fabsf(offset.z) > bounds.z) {
            continue;
        }
        counts[pass]++;
        const double made_life = zox_getv(e, DestroyInTime);
        wrong += !color_equals(zox_getv(e, Color), parent_colors[pass]);
        wrong += !float3_equals(zox_getv(e, Gravity3D), ((float3) { 0, rise, 0 }));
        wrong += made_life < life * 0.7f || made_life > life || fabs(made_life - zox_getv(e, FadeOutTime)) > 1e-6;
    zox_each_end();
    zox_tst_check(counts[0] == rate && counts[1] == rate && !wrong,
        "stream groups had [%i,%i] particles in bounds and [%i] bad colors, gravity or lifetimes", counts[0], counts[1], wrong);

    const int before_stall = zox_count_types(Particle3D);
    zox_tst_particle_stream_ticks(world, 4, 12.03125);
    const int after_stall = zox_count_types(Particle3D) - before_stall;
    zox_tst_check(after_stall >= 8 && after_stall <= 12, "four stalled frames emitted [%i] particles despite the 100 ms cap", after_stall);
}
