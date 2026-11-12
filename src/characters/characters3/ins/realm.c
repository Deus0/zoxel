entity spawn_character3_meta(
    ecs *world,
    entity prefab,
    entity realm,
    lint seed,
    const char* name,
    byte chance)
{
    // a prefab variant (a prefab first, then its base), made an instance then a prefab, its instances lost
    //  their base's children (the skeleton's body slot, humanoids were invisible) once it had non
    //  fragmenting children (its stats), flecs asserts adding them (tst prefab_variants, npc_skeletons)
    const entity e = zox_prefab_variant(world, prefab);
    setup_character3(world, e, prefab, realm, 0, seed, 0, 0, float3_zero, quaternion_identity, name);
    zox_name(name);
    zox_setv(e, SpawnChance, chance);
    return e;
}
