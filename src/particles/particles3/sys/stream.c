// a streamer: an emitter (ParticleStreamer) letting out ParticlesPerSecond, frame rate free (ParticlesDue its carried
//  part), not ParticleEmitRate's per frame: its Color, each living ParticleLife, in its Bounds3D (half extents) and
//  rising at ParticleRise; a child, it follows its parent (a projectile's trail, a burn's flames), its parent's Color
//  with ParticleParentColor (an instance's copy of its prefab's streamer, coloured as it's shot)
void particle3_stream_system(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    const float dt = delta_time > 0.1f ? 0.1f : delta_time;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Bounds3D);
    zox_sys_in(Color);
    zox_sys_in(ParticlesPerSecond);
    zox_sys_in(ParticleLife);
    zox_sys_in(ParticleRise);
    zox_sys_out(ParticlesDue);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_i(Color, c);
        zox_sys_i(ParticlesPerSecond, rate);
        zox_sys_i(ParticleLife, life);
        zox_sys_i(ParticleRise, rise);
        zox_sys_o(ParticlesDue, due);
        due->value += rate->value * dt;
        color particle_color = c->value;
        if (zox_has(e, ParticleParentColor)) {
            const entity parent = zox_get_parent(world, e);
            if (zox_valid(parent) && zox_has(parent, Color)) {
                particle_color = zox_getv(parent, Color);
            }
        }
        const float3 s = bounds->value;
        for (; due->value >= 1; due->value -= 1) {
            const float3 p = float3_add(position->value, (float3) { randf_range(-s.x, s.x), randf_range(-s.y, s.y), randf_range(-s.z, s.z) });
            const entity particle = spawn_particle3(world, p, float3_zero, particle_color);
            zox_setv(particle, Velocity3D, ((float3) { randf_range(-0.2f, 0.2f), rise->value * randf_range(0.6f, 1.2f), randf_range(-0.2f, 0.2f) }));
            zox_setv(particle, Gravity3D, float3_zero);
            zox_setv(particle, DestroyInTime, life->value * randf_range(0.7f, 1.0f));
        }
    }
    zox_sys_on_end();
} zoxd_system(particle3_stream_system);
