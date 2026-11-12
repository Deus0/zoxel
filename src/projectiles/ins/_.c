// Spawns a projectile flying forward from rotation
//  - hits are handled by projectile_hit_system (skills), raycasting along its velocity each frame
entity spawn_projectile(
    ecs* world,
    entity prefab,
    entity user,
    entity skill,
    float3 position,
    float4 rotation,
    float scale,
    float power,
    float damage,
    color projectile_color)
{
    entity e = spawn_cube(
        world,
        prefab,
        position,
        scale);
    if (!e) {
        return 0;
    }
    zox_set(e, Rotation3D, { rotation });
    zox_setv(e, Color, projectile_color);
    // set force here
    float3 force = float4_rotate_float3(rotation, (float3) { 0, 0, -power });
    zox_set(e, Acceleration3D, { force });
    zox_set(e, Damage, { damage });
    zox_set_link(world, e, ProjectileUser, user);
    zox_set_link(world, e, ProjectileSkill, skill);
    // terrain to collide with
    entity terrain = zox_valid(user) ?
        zox_get_link(world, user, TerrainLink) :
        0;
    zox_set_link(world, e, TerrainLink, terrain);
    return e;
}
