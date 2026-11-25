// tst: particle_emit, a frame emitter emits its configured budget every update and stops at rate zero.
static void zox_tst_particle_emit_check(ecs* world) {
    const int rate = 7;
    const float3 centre = { -64, 20, 36 };
    const float3 size = { 1, 2, 4 };
    const color tint = { 100, 120, 240, 220 };
    const entity emitter = spawn_particle3D_emitter_at(world, centre, rate, size, tint);
    const int before = zox_count_types(Particle3D);
    for (int frame = 1; frame <= 2; frame++) {
        zox_run(world, zox_id(particle3d_emit_system));
        const int made = zox_count_types(Particle3D) - before;
        zox_tst_check(made == frame * rate, "after emit frame [%i], [%i] of [%i] particles spawned", frame, made, frame * rate);
    }
    int matched = 0, wrong_color = 0;
    zox_each_begin(Particle3D);
        const float3 offset = float3_sub(zox_getv(e, Position3D), centre);
        if (fabsf(offset.x) > size.x / 2 || fabsf(offset.y) > size.y / 2 || fabsf(offset.z) > size.z / 2) {
            continue;
        }
        matched++;
        const color c = zox_getv(e, Color);
        wrong_color += abs(c.r - tint.r) > 1 || abs(c.g - tint.g) > 1 || abs(c.b - tint.b) > 1 || c.a > tint.a || c.a < tint.a - 52;
    zox_each_end();
    zox_tst_check(matched == 2 * rate && wrong_color == 0, "[%i] particles in the emitter bounds, [%i] bad colors", matched, wrong_color);
    zox_setv(emitter, ParticleEmitRate, 0);
    const int stopped = zox_count_types(Particle3D);
    zox_run(world, zox_id(particle3d_emit_system));
    zox_tst_check(zox_count_types(Particle3D) == stopped, "rate zero kept emitting");
}
