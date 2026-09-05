entity spawn_projectile(
    ecs* world,
    entity prefab,
    float3 position,
    float4 rotation,
    float scale,
    float power,
    float damage)
{
    entity e = spawn_cube(
        world,
        prefab,
        position,
        scale);
    // set transform
    // zox_set(e, Position3D, { position });
    zox_set(e, Rotation3D, { rotation });
    // set force here
    float3 force = float4_rotate_float3(rotation, (float3) { 0, 0, -power });
    zox_set(e, Acceleration3D, { force });
    zox_set(e, Damage, { damage });
    return e;
}
