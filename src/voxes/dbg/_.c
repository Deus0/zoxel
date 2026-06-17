// Toggle Terrains Bounds
extern entity prefab_chunk_terrain;
extern entity get_linked_terrain(ecs*, entity);

void zox_dbg_toggle_gizmos_terrain(ecs* world, ClickEventData data) {
    entity terrain = local_terrain;
    if (!zox_valid(terrain)) {
        return;
    }
    byte mode = zox_get_value(prefab_chunk_terrain, DebugCubeLines);
    cycle_cubeline_debug(&mode);
    zox_set(prefab_chunk_terrain, DebugCubeLines, { mode });
    zox_geter(terrain, ChunkLinks, chunks);
    for (uint i = 0; i < chunks->value->size; i++) {
        int3_hashmap_pair* pair = chunks->value->data[i];
        uint checks = 0;
        while (pair != NULL && checks < max_safety_checks_hashmap) {
            entity c = pair->value;
            if (zox_valid(c)) {
                zox_set(c, DebugCubeLines, { mode });
            }
            pair = pair->next;
            checks++;
        }
    }
}
