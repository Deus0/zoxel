entity spawn_prefab_projectile(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_add(e, Projectile);
    zox_set(e, DestroyInTime, { 3 });
    zox_set(e, Damage, { 0 });
    // Add Physics and Forward Vector
    zox_add(e, Moveable);
    zox_set(e, Velocity3D, { float3_zero });
    zox_setv(e, ClockTime, 0);      // its terrain's (ClockLink, as it's shot)
    zox_setv(e, ClockDelta, 0);
    zox_set(e, Acceleration3D, { float3_zero });
    zox_set(e, Gravity3D, { float3_multiply1(zox_default_gravity, 0.2f) });
    // its trail (coloured as it's shot)
    add_prefab_particle3_streamer(world, e, 50, 0.35f, float3_single(0.04f), 0.4f);
    return e;
}

entity prefab_projectile;

void zox_define_prefabs_projectiles(ecs* world) {
    prefab_projectile = spawn_prefab_projectile(world, prefab_cube);
}
