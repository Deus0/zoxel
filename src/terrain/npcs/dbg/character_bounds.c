extern entity get_linked_terrain(ecs*, entity);

void toggle_debug_character_bounds(ecs *world) {

    const entity realm = local_realm;
    if (!zox_valid(realm)) return;
    const entity terrain = get_linked_terrain(world, realm);
    if (!zox_valid(terrain)) return;

    byte mode = zox_get_value(prefab_character3, DebugCubeLines);
    cycle_cubeline_debug(&mode);
    zox_prefab_character_set(DebugCubeLines, { mode });
    // zox_set(prefab_character3_npc, DebugCubeLines, { mode })
    zox_geter(terrain, ChunkLinks, chunkLinks);
    for (uint i = 0; i < chunkLinks->value->size; i++) {
        int3_hashmap_pair* pair = chunkLinks->value->data[i];
        uint checks = 0;
        while (pair != NULL && checks < max_safety_checks_hashmap) {
            entity chunk = pair->value;
            zox_geter(chunk, ChunkEntities, entityLinks)
            for (int j = 0; j < entityLinks->length; j++) {
                entity e2 = entityLinks->value[j];
                zox_set(e2, DebugCubeLines, { mode })
            }
            pair = pair->next;
            checks++;
        }
    }
}

void key_down_toggle_debug_character_bounds(ecs *world, int32_t keycode) {
    if (keycode == SDLK_F5) {
        toggle_debug_character_bounds(world);
    }
}