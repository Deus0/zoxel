entity spawn_prefab_particle3D_emitter(ecs *world) {
    zox_prefab();
    zox_prefab_name("particle3D_emitter");
    zox_add(e, Particle3DEmitter);
    zox_prefab_set(e, ParticleEmitRate, { 0 });
    // attach to parent character
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { quaternion_identity });
    zox_prefab_set(e, Scale1, { 1 });
    // zox_prefab_set(e, LocalPosition3D, { float3_zero });
    // zox_prefab_set(e, LocalRotation3D, { quaternion_identity });
    zox_prefab_set(e, Bounds3D, { 0 });
    zox_prefab_set(e, Color, { color_white });
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

