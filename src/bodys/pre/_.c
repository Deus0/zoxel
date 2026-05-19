void spawn_prefabs_bodys(ecs* world) {
    if (prefab_character3) {
        zox_prefab_character_add(BodySize);
        zox_prefab_character_add(BodyDirty);
        zox_prefab_character_add(CombineVox);
        zox_prefab_character_add(CombineList);
        zox_prefab_character_add(CombinePositions);
        // Spawn Body Slots on Character3 Skeleton
        entity body = spawn_slot(world, prefab_character3_skeleton);
        zox_set_unique_name(body, "body");
        zox_add_tag(body, Body);
    }
}
