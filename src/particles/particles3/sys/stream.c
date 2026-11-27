// a streamer: an emitter (ParticleStreamer) letting out ParticleEmitRate a second, not a frame: in its parent's Color
//  (a shot's, a burn's skill's: an instance's copy of its prefab's streamer, made in a system, can't be coloured
//  then), else its own; as bright as it (Brightness: past 1 they glow); each living ParticleLife, in its Bounds3D (half extents), falling by its Gravity3D (up rises)
//  - a child, it follows its parent (a projectile's trail, a burn's flames)
//  - how many a frame from the time (its rate's whole steps passed), no carried part kept
void particle3_stream_system(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    const double now = zox_current_time;
    const double before = now - (delta_time > 0.1 ? 0.1 : delta_time);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Bounds3D);
    zox_sys_in(Color);
    zox_sys_in(ParticleEmitRate);
    zox_sys_in(ParticleLife);
    zox_sys_in(Gravity3D);
    zox_sys_in(Brightness);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_i(Color, c);
        zox_sys_i(ParticleEmitRate, rate);
        zox_sys_i(ParticleLife, life);
        zox_sys_i(Gravity3D, gravity);
        zox_sys_i(Brightness, brightness);
        const double phase = (double) (e % 97) / 97.0;     // each its own offset
        const int count = (int) (floor(now * rate->value + phase) - floor(before * rate->value + phase));
        if (count <= 0) {
            continue;
        }
        const entity parent = zox_get_parent(world, e);
        const color particle_color = zox_valid(parent) && zox_has(parent, Color) ? zox_getv(parent, Color) : c->value;
        const float3 s = bounds->value;
        for (int j = 0; j < count; j++) {
            const float3 p = float3_add(position->value, (float3) { randf_range(-s.x, s.x), randf_range(-s.y, s.y), randf_range(-s.z, s.z) });
            const entity particle = spawn_particle3(world, p, float3_zero, particle_color);
            zox_setv(particle, Velocity3D, ((float3) { randf_range(-0.2f, 0.2f), 0, randf_range(-0.2f, 0.2f) }));
            zox_setv(particle, Gravity3D, gravity->value);
            zox_setv(particle, DestroyInTime, life->value * randf_range(0.7f, 1.0f));
            zox_setv(particle, Brightness, brightness->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(particle3_stream_system);
