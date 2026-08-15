entity spawn_prefab_particle3(ecs *world) {
    zox_prefab();
    // zox_prefab_name("particle3");
    zox_add(e, Particle);
    zox_add(e, Particle3D);
    zox_add(e, Moveable);
    zox_add(e, DisableParentTransform);
    zox_prefab_set(e, Position3D, { float3_zero });
    // zox_prefab_set(e, Rotation3D, { quaternion_identity });
    // zox_prefab_set(e, Scale1, { 1 });
    zox_prefab_set(e, Velocity3D, { float3_zero });
    zox_prefab_set(e, Acceleration3D, { float3_zero });
    zox_prefab_set(e, Brightness, { 1 });
    zox_prefab_set(e, Color, {{ 255, 0, 0, 125 }});
    zox_prefab_set(e, DestroyInTime, { 3 });
    zox_prefab_set(e, Gravity3D, { (float3) { 0, 0.1f, 0 } });
    return e;
}

entity spawn_particle3(ecs *world, float3 position, float3 acceleration, color particle_color) {
    zox_instance(prefab_particle3D);
    // zox_name("particle3D")
    zox_setv(e, Position3D, position);
    zox_setv(e, Acceleration3D, acceleration);
    zox_setv(e, Color, particle_color);
    // zox_setv(e, Velocity3D, float3_up);
    return e;
}
