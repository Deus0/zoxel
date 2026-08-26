zox_sys2(Tunk2DeathSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(RenderDistance);
    zox_sys_in(TunkPosition);
    zox_sys_in(Chunk3Stack);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDistanceDirty, dirty);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(TunkPosition, position);
        zox_sys_i(Chunk3Stack, stack);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        // Pass if loading chunk
        if (distance->value == 255) {
            continue;
        }
        // Pass if lod changing
        byte is_kill = distance->value > terrain_lod_far;
        if (!is_kill) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain) || !zox_has(terrain, TunkLinks)) {
            zox_logw("Tunk has no Parent Terrain [%s]", zox_get_name(e));
            zox_delete(e);
            continue;
        }
        // Remove Chunks too
        zox_muter(terrain, ChunkLinks, chunks);
        int k = 0;
        for (int j = -render_distance_y; j <= render_distance_y; j++, k++) {
            entity chunk = stack->value[k];
            if (!zox_valid(chunk)) {
                continue;
            }
            int3 position3 = (int3) {
                position->value.x,
                j,
                position->value.y
            };
            int3_hashmap_remove(chunks->value, position3);
            zox_delete(chunk);
        }
        // remove from hash - can i do this better?
        zox_muter(terrain, TunkLinks, tunks);
        int2_hashmap_remove(tunks->value, position->value);
        zox_delete(e);
        // zox_log("Deleted Tunk2 [%ix%i]", position->value.x, position->value.y);
    }
} zox_sys_end(Tunk2DeathSystem);
