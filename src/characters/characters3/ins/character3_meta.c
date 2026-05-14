entity spawn_character3_meta(ecs *world, entity prefab, entity spawn_prefab, const char* name, entity model, byte chance) {
    zox_instance(prefab);
    zox_name(name); // "character3_meta");
    zox_set(e, ModelLink, { model });
    zox_set(e, SpawnChance, { chance });
    zox_set(e, Character3PrefabLink, { spawn_prefab });
    return e;
}
