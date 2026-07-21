// NOTE: Sets active or not
zox_sys2(Characters3SpawnZoneSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChunkLodDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDistance);
    zox_sys_in(GenerateChunk);
    zox_sys_out(NpcSpawnZone);
    zox_sys_out(NpcSpawnZoneDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkLodDirty, lod_dirty);
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(RenderDistance, distance);
        zox_sys_i(GenerateChunk, generate);
        zox_sys_o(NpcSpawnZone, active);
        zox_sys_o(NpcSpawnZoneDirty, dirty);
        // NOTE: Lod Dirty is a short state so it wont catch it now
        if (!lod_dirty->value) {
            // continue;
        }
        // Max Depth Checks
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Terrain missing on chunk3_terrain");
            continue;
        }
#endif
        float terrain_depth = zox_getv(terrain, NodeDepth);
        byte new_active = !generate->value && depth->value == terrain_depth && distance->value <= character_spawn_distance;
        if (active->value != new_active) {
            active->value = new_active;
            dirty->value = zox_dirty_trigger;
            continue;
        }
    }
} zox_sys_end(Characters3SpawnZoneSystem);
