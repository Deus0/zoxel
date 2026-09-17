entity spawn_character3_meta(
    ecs *world,
    entity prefab,
    entity realm,
    lint seed,
    const char* name,
    byte chance)
{
    entity e = spawn_character3(
        world,
        prefab,
        realm,
        0,
        seed,
        0,
        0,
        float3_zero,
        quaternion_identity,
        name);
    zox_make_prefab(e);
    zox_name(name);
    zox_setv(e, SpawnChance, chance);
    return e;
}
