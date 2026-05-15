entity spawn_character3_meta(ecs *world, entity prefab, const char* name, entity model, byte chance) {
    entity e = spawn_character3(world, prefab, 0, 0, model, 0, 0, float3_zero, quaternion_identity, name);
    zox_name(name);
    zox_set(e, SpawnChance, { chance });
    zox_make_prefab(e);
    return e;
}
