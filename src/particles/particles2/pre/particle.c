entity spawn_prefab_particle2(ecs* world) {
    zox_prefab();
    // zox_prefab_name("particle2");
    zox_add_tag(e, Particle);
    zox_add_tag(e, Particle2D);
    zox_prefab_set(e, Position2, { float2_zero });
    zox_prefab_set(e, Rotation2D, { 0 });
    zox_prefab_set(e, Scale1, { 1 });
    zox_prefab_set(e, Velocity2D, {{ 0, 0 }});
    zox_prefab_set(e, Acceleration2D, {{ 0, 0 }});
    zox_prefab_set(e, Torque2D, { 0 });
    zox_prefab_set(e, Bounds2D, { { 0, 0 } });
    zox_prefab_add(e, Brightness);
    zox_prefab_add(e, DestroyInTime);
    return e;
}
