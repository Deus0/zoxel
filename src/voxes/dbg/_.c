// Toggle Terrains Bounds
extern entity prefab_chunk_terrain;
extern entity get_linked_terrain(ecs*, entity);

void toggle_debug_bounds_terrain(ecs* world) {

    const entity realm = local_realm;
    if (!zox_valid(realm)) return;
    const entity terrain = get_linked_terrain(world, realm);
    if (!zox_valid(terrain)) return;

    byte mode = zox_get_value(prefab_chunk_terrain, DebugCubeLines);

    cycle_cubeline_debug(&mode);

    is_render_chunk_edges = mode;
    zox_set(prefab_chunk_terrain, DebugCubeLines, { mode });
    zox_geter(terrain, ChunkLinks, chunkLinks);
    for (uint i = 0; i < chunkLinks->value->size; i++) {
        int3_hashmap_pair* pair = chunkLinks->value->data[i];
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

void key_down_toggle_debug_chunks(ecs* world, int32_t keycode) {
    if (keycode == SDLK_F4) {
        toggle_debug_bounds_terrain(world);
    }
}