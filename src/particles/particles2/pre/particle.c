entity spawn_prefab_particle2(ecs* world) {
    zox_prefab();
    // zox_prefab_name("particle2");
    zox_add(e, Particle);
    zox_add(e, Particle2D);
    zox_set(e, Position2, { float2_zero });
    zox_set(e, Rotation2, { 0 });
    zox_set(e, Scale1, { 1 });
    zox_set(e, Velocity2D, {{ 0, 0 }});
    zox_set(e, Acceleration2D, {{ 0, 0 }});
    zox_set(e, Torque2D, { 0 });
    zox_set(e, Bounds2D, { { 0, 0 } });
    zox_add(e, Brightness);
    zox_add(e, DestroyInTime);
    return e;
}
