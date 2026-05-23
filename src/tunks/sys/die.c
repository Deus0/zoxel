zox_sys2(Tunk2DeathSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxLink);
    zox_sys_in(Chunk2Position);
    zox_sys_in(RenderDistance);
    // zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(Chunk2Position, position);
        // zox_sys_i(RenderDepth, depth);
        zox_sys_i(RenderDistance, distance);
        if (!zox_valid(terrain->value) || !zox_has(terrain->value, TunkLinks)) {
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
        zox_muter(terrain->value, TunkLinks, chunks);
        // zox_geter(terrain->value, ChunkLinks, chunks);
        int2_hashmap_remove(chunks->value, position->value);
        zox_delete(e);
        // zox_log("Deleted Tunk2 [%ix%i]", position->value.x, position->value.y);
    }
} zox_sys_end(Tunk2DeathSystem);
