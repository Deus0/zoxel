// NOTE: On chunk death we remove it from the hashmap of parent
// NOTE: We are lucky they die by stream so there is a single point for their deaths
zox_sys2(Chunk3DeathSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(RenderDistance);
    zox_sys_in(ChunkPosition);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDistanceDirty, dirty);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(ChunkPosition, position);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        // Pass if lod changing
        byte is_kill = distance->value > terrain_lod_far;
        if (!is_kill) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_delete(e);
            continue;
        }
#endif
        // remove from hash - can i do this better?
        zox_muter(terrain, ChunkLinks, chunks);
        int3_hashmap_remove(chunks->value, position->value);
        zox_delete(e);
    }
} zox_sys_end(Chunk3DeathSystem);
