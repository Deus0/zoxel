entity spawn_body_slot(ecs* world, entity parent, byte anchor) {
    entity e = spawn_slot(world, parent);
    zox_set_unique_name(e, "body_slot");
    zox_set(e, SlotAnchor, { anchor });
    zox_set(e, PartPosition, { byte3_zero });
    zox_set(e, PartSize, { byte3_zero });
    zox_set(e, PartOffset, { int3_zero });
    return e;
}

void spawn_prefabs_bodys(ecs* world) {
    if (prefab_character3) {
        zox_prefab_character_add(BodySize);
        zox_prefab_character_add(BodyDirty);
        zox_prefab_character_add(CombineVox);
        zox_prefab_character_add(CombineList);
        zox_prefab_character_add(CombinePositions);
        // Spawn Body Slots on Character3 Skeleton
        entity body_slot = spawn_body_slot(world, prefab_character3_skeleton, body_anchor_core);
        zox_add_tag(body_slot, Body);

    }
}
