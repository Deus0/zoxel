entity spawn_prefab_particle3D_emitter(ecs *world) {
    zox_prefab();
    zox_prefab_name("particle3D_emitter");
    zox_add(e, Particle3DEmitter);
    zox_set(e, ParticleEmitRate, { 0 });
    // attach to parent character
    zox_set(e, Position3D, { float3_zero });
    zox_set(e, Rotation3D, { quaternion_identity });
    zox_set(e, Scale1, { 1 });
    // zox_set(e, LocalPosition3D, { float3_zero });
    // zox_set(e, LocalRotation3D, { quaternion_identity });
    zox_set(e, Bounds3D, { 0 });
    zox_set(e, Color, { color_white });
    return e;
}

entity spawn_particle3D_emitter(ecs *world, entity parent, int emit_rate, float3 bounds, color particles_color) {
    zox_instance(prefab_particle3D_emitter);
    zox_name("particle3D_emitter");
    zox_set(e, ParticleEmitRate, { emit_rate });
    zox_set(e, Bounds3D, { bounds });
    zox_set(e, Color, { particles_color });
    zox_set_parent(world, e, parent);
    return e;
}

entity spawn_particle3D_emitter_at(ecs *world, float3 position, int emit_rate, float3 bounds, color particles_color) {
    zox_instance(prefab_particle3D_emitter);
    zox_name("particle3D_emitter");
    zox_set(e, Position3D, { position });
    zox_set(e, ParticleEmitRate, { emit_rate });
    zox_set(e, Bounds3D, { bounds });
    zox_set(e, Color, { particles_color });
    return e;
}


// a streamer (sys/stream.c), rate a second: a prefab child of a prefab (a projectile's trail, a burn's flames), its
//  instances' copies follow them in their Color; each living life, in bounds (half extents), rising at rise
entity add_prefab_particle3_streamer(ecs* world, entity parent, int rate, float life, float3 bounds, float rise) {
    const entity e = zox_prefab_variant(world, prefab_particle3D_emitter);
    zox_add(e, ParticleStreamer);
    zox_set(e, ParticleEmitRate, { rate });
    zox_set(e, ParticleLife, { life });
    zox_set(e, Gravity3D, { (float3) { 0, rise, 0 } });    // its particles': up, they rise
    zox_set(e, Bounds3D, { bounds });
    zox_set_parent(world, e, parent);
    return e;
}

// an instance's streamer (its prefab's copy), 0 none
static inline entity get_particle3_streamer(ecs* world, entity e) {
    return zox_get_child_by_id(world, e, zox_id(ParticleStreamer));
}
