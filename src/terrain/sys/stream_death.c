zox_sys2(Chunk3DeathSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_in(RenderDistance);
    zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(ChunkPosition, position);
        if (depth->value == render_depth_uninitialized) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain->value)) {
            zox_delete(e)
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
        // remove from hash - can i do this better?
        zox_muter(terrain, ChunkLinks, chunks);
        int3_hashmap_remove(chunks->value, position->value);
        zox_delete(e);
    }
} zox_sys_end(Chunk3DeathSystem);
