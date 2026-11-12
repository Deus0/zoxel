// NOTE: Sets the chunks NpcSpawnZone active or not
//  - becomes active when the chunk is close, at full detail, lod settled, and finished generating
//  - stays active until it's one chunk past range or below full detail (lod transitions don't despawn npcs)
//  - checked every frame, as lod changes and generation finish at different times
void characters_can_spawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDistance);
    zox_sys_out(NpcSpawnZone);
    zox_sys_out(NpcSpawnZoneDirty);
    entity cached_terrain = 0;
    byte terrain_depth = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(RenderDistance, distance);
        zox_sys_o(NpcSpawnZone, active);
        zox_sys_o(NpcSpawnZoneDirty, dirty);
        // wait for the last change to finish
        if (dirty->value) {
            continue;
        }
        // hysteresis: spawns within character_spawn_distance, despawns one chunk past it,
        //  - so a streamer bobbing across a chunk border (jumping, landing) doesn't despawn + respawn npcs
        const byte in_distance = active->value ?
            distance->value <= character_spawn_distance + 1 :
            distance->value <= character_spawn_distance;
        // cheap early out, most chunks are far away
        if (!in_distance && !active->value) {
            continue;
        }
        byte new_active = in_distance;
        if (new_active) {
            entity terrain = zox_get_parent(world, e);
            if (!zox_valid(terrain)) {
                continue;
            }
            if (terrain != cached_terrain) {
                cached_terrain = terrain;
                terrain_depth = zox_getv(terrain, NodeDepth);
            }
            const byte lod_state = zox_has(e, ChunkLodDirty) ?
                zox_getv(e, ChunkLodDirty) :
                zox_chunk_lod_dirty_end;
            const byte lod_settled =
                lod_state == zox_chunk_lod_dirty_end ||
                lod_state >= zox_chunk_lod_dirty_spawn;
            const byte full_detail =
                depth->value == terrain_depth &&
                zox_getv(e, NodeDepth) == terrain_depth;
            if (active->value) {
                // NOTE: once active, only leaving range or dropping detail deactivates it
                //  - walking re-dirties lods of nearby chunks, which used to despawn and respawn their npcs
                new_active = full_detail;
            } else {
                new_active =
                    full_detail &&
                    lod_settled &&
                    !zox_has(e, GenerateChunk);
            }
        }
        if (active->value != new_active) {
            active->value = new_active;
            dirty->value = zox_dirty_trigger;
        }
    }
    zox_sys_on_end();
} zoxd_system(characters_can_spawn_system);
