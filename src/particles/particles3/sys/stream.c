// a stream of particles (ParticlesPerSecond, frame rate free, ParticlesDue its carried part): its Color, each living
//  ParticleLife, spread in ParticleSpread (half extents) and rising at ParticleRise
//  - at its Position3D, or its parent's without one (a debuff on a character)
void particle3_stream_system(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    const float dt = delta_time > 0.1f ? 0.1f : delta_time;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ParticlesPerSecond);
    zox_sys_in(ParticleLife);
    zox_sys_in(ParticleSpread);
    zox_sys_in(ParticleRise);
    zox_sys_in(Color);
    zox_sys_out(ParticlesDue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ParticlesPerSecond, rate);
        zox_sys_i(ParticleLife, life);
        zox_sys_i(ParticleSpread, spread);
        zox_sys_i(ParticleRise, rise);
        zox_sys_i(Color, c);
        zox_sys_o(ParticlesDue, due);
        const entity at = zox_has(e, Position3D) ? e : zox_get_parent(world, e);
        if (!zox_valid(at) || !zox_has(at, Position3D)) {
            continue;
        }
        due->value += rate->value * dt;
        const float3 position = zox_getv(at, Position3D);
        const float3 s = spread->value;
        for (; due->value >= 1; due->value -= 1) {
            const float3 p = float3_add(position, (float3) { randf_range(-s.x, s.x), randf_range(-s.y, s.y), randf_range(-s.z, s.z) });
            const entity particle = spawn_particle3(world, p, float3_zero, c->value);
            zox_setv(particle, Velocity3D, ((float3) { randf_range(-0.2f, 0.2f), rise->value * randf_range(0.6f, 1.2f), randf_range(-0.2f, 0.2f) }));
            zox_setv(particle, Gravity3D, float3_zero);
            zox_setv(particle, DestroyInTime, life->value * randf_range(0.7f, 1.0f));
        }
    }
    zox_sys_on_end();
} zoxd_system(particle3_stream_system);

// a particle stream added to an entity (its colour its own Color, or this)
void add_particle3_stream(ecs* world, entity e, float rate, float life, float3 spread, float rise, color c) {
    zox_set(e, ParticlesPerSecond, { rate });
    zox_set(e, ParticlesDue, { 0 });
    zox_set(e, ParticleLife, { life });
    zox_set(e, ParticleSpread, { spread });
    zox_set(e, ParticleRise, { rise });
    zox_set(e, Color, { c });
}
