// Spawns a projectile flying forward from rotation, at speed (units a second)
//  - hits are handled by projectile_hit_system (skills), raycasting along its velocity each frame
entity spawn_projectile(
    ecs* world,
    entity prefab,
    entity user,
    entity skill,
    float3 position,
    float4 rotation,
    float scale,
    float speed,
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
    // launched at its speed: a force for a frame was slow (1.4 a second) and its frame rate's
    zox_set(e, Velocity3D, { float4_rotate_float3(rotation, (float3) { 0, 0, -speed }) });
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
