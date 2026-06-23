entity spawn_prefab_projectile(ecs* world, entity prefab) {
    entity e = zox_prefab_from_parent(world, prefab);
    zox_add_tag(e, Projectile);
    zox_prefab_set(e, DestroyInTime, { 3 });
    // Add Physics and Forward Vector
    zox_add_tag(e, Moveable);
    zox_prefab_set(e, Velocity3D, { float3_zero });
    zox_prefab_set(e, Acceleration3D, { float3_zero });
    // zox_add_tag(e, Frictioned);
    // zox_prefab_set(e, InitializePhysics3D, { 0 });
    // zox_prefab_set(e, LastPosition3D, { float3_zero });
    // zox_prefab_set(e, LastUnstuck3, { float3_zero });
    // zox_prefab_set(e, DisableGravity, { 0 });
    // zox_prefab_set(e, Gravity3D, { zox_default_gravity });
    // zox_prefab_set(e, Omega3D, { float3_zero });
    // zox_prefab_set(e, Alpha3D, { float3_zero });
    return e;
}

entity prefab_projectile;

void zox_define_prefabs_projectiles(ecs* world) {
    prefab_projectile = spawn_prefab_projectile(world, prefab_cube);
}
