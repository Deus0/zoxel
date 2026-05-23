// Streamer components added to cameras
zox_sys2(FirstTerrainChunkSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StreamLink);
    zox_sys_in(StreamPoint);
    zox_sys_in(StreamDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StreamLink, terrain);
        zox_sys_i(StreamPoint, position);
        zox_sys_i(StreamDirty, dirty);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(terrain->value)) {
            continue;
        }
        zox_mut_begin(terrain->value, ChunkLinks, chunks3);
        // No need to spawn if exists in links
        if (int3_hashmap_has(chunks3->value, position->value)) {
            continue;
        }
        if (!zox_has(terrain->value, BlockScale) || !zox_has(terrain->value, NodeDepth)) {
            continue;
        }
        zox_geter_value(terrain->value, BlockScale, float, tscale);
        zox_geter_value(terrain->value, NodeDepth, byte, tdepth);
        entity chunk = spawn_chunk_terrain(world, prefab_chunk_terrain, terrain->value, position->value, position->value, tdepth, tscale);
        if (!zox_valid(chunk)) {
            zox_loge("Failed to spawn chunk [%ix%ix%i] on %s", position->value.x, position->value.y, position->value.z, zox_get_name(terrain->value));
            continue;
        }
        int3_hashmap_add(chunks3->value, position->value, chunk);
        zox_mut_end(terrain->value, ChunkLinks);
    }
} zox_sys_end(FirstTerrainChunkSystem);
